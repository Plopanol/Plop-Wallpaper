#include "resource.h"
#include "error_log.h"
#include "monitor_retry.h"
#include "settings.h"
#include "startup.h"
#include "theme.h"
#include "tray_click.h"

#include <algorithm>
#include <commctrl.h>
#include <dbt.h>
#include <memory>
#include <shellapi.h>
#include <windowsx.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace {

constexpr wchar_t appName[] = L"Plop-Wallpaper";
constexpr int monitorListId = 100;
constexpr int chooseFolderId = 101;
constexpr int changeWallpaperId = 102;
constexpr int folderPathId = 103;
constexpr int statusId = 104;
constexpr int fitId = 105;
constexpr int rotationModeId = 106;
constexpr int uniqueModeId = 107;
constexpr int minutesId = 108;
constexpr int applyRotationId = 109;
constexpr int startupId = 110;
constexpr int chooseImageId = 111;
constexpr int imagePathId = 112;
constexpr int randomId = 113;
constexpr int monitorLabelId = 114;
constexpr int rotationSectionId = 115;
constexpr int uniqueSectionId = 116;
constexpr int intervalLabelId = 117;
constexpr int closeToTrayId = 118;
constexpr UINT_PTR rotationTimer = 1;
constexpr UINT_PTR monitorTimer = 2;
constexpr UINT_PTR trayClickTimer = 3;
constexpr UINT monitorRefreshDelay = 250;
constexpr UINT monitorRetryDelay = 2000;
constexpr UINT trayMessage = WM_APP + 1;
constexpr UINT trayIconId = 1;

void showError(HWND owner, PCWSTR message, HRESULT result)
{
    wchar_t code[16]{};
    swprintf_s(code, L"0x%08lX", static_cast<unsigned long>(result));
    std::wstring detail = std::wstring(message) + L" (" + code + L")";
    logError(detail.c_str());
    MessageBoxW(owner, detail.c_str(), appName, MB_OK | MB_ICONERROR);
}

HWND createControl(HWND window, PCWSTR type, PCWSTR text, DWORD style,
                   int x, int y, int width, int height, int id = 0)
{
    if (lstrcmpW(type, L"BUTTON") == 0 && (style & BS_TYPEMASK) == BS_PUSHBUTTON) {
        style |= BS_OWNERDRAW;
    }
    HWND control = CreateWindowW(type, text, WS_CHILD | WS_VISIBLE | style,
                                 x, y, width, height, window,
                                 reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                                 nullptr, nullptr);
    if (control) {
        SendMessageW(control, WM_SETFONT, reinterpret_cast<WPARAM>(GetStockObject(DEFAULT_GUI_FONT)), TRUE);
    }
    return control;
}

struct WindowState {
    explicit WindowState(std::filesystem::path executablePath)
        : executable(std::move(executablePath)),
          cache(executable.parent_path() / L"fonds-générés"),
          settings(executable.parent_path() / L"config.ini")
    {
    }

    std::vector<Monitor> monitors;
    ComPtr<IDesktopWallpaper> wallpaper;
    ComPtr<IWICImagingFactory> imaging;
    std::filesystem::path executable;
    std::filesystem::path cache;
    Settings settings;
    RotationSettings rotation;
    std::mt19937 rotationRandom{std::random_device{}()};
    WindowTheme theme;
    std::vector<MonitorSettings> selections;
    UINT taskbarCreated = 0;
    bool inTray = false;
    bool closeToTray = false;
    TrayClick trayClick;
    bool handlingCommand = false;
    bool monitorsPending = false;
    MonitorRetry monitorRetry;
    bool rotationScheduled = false;
};

NOTIFYICONDATAW trayIcon(HWND window)
{
    NOTIFYICONDATAW icon{};
    icon.cbSize = sizeof(icon);
    icon.hWnd = window;
    icon.uID = trayIconId;
    return icon;
}

bool addTrayIcon(HWND window)
{
    auto icon = trayIcon(window);
    icon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    icon.uCallbackMessage = trayMessage;
    icon.hIcon = reinterpret_cast<HICON>(GetClassLongPtrW(window, GCLP_HICON));
    lstrcpynW(icon.szTip, appName, ARRAYSIZE(icon.szTip));
    if (Shell_NotifyIconW(NIM_ADD, &icon)) {
        icon.uVersion = NOTIFYICON_VERSION_4;
        if (Shell_NotifyIconW(NIM_SETVERSION, &icon)) {
            return true;
        }
        Shell_NotifyIconW(NIM_DELETE, &icon);
    }
    logError(L"Impossible d’ajouter l’icône de notification.");
    return false;
}

void hideToTray(HWND window, WindowState& state)
{
    if (!state.inTray && state.taskbarCreated && addTrayIcon(window)) {
        state.inTray = true;
        if (IsWindowVisible(window)) {
            ShowWindow(window, SW_HIDE);
        }
    }
}

void restoreWindow(HWND window, WindowState& state)
{
    KillTimer(window, trayClickTimer);
    state.trayClick.cancel();
    state.inTray = false;
    auto icon = trayIcon(window);
    Shell_NotifyIconW(NIM_DELETE, &icon);
    ShowWindow(window, SW_RESTORE);
    SetForegroundWindow(window);
}

void showTrayMenu(HWND window, WindowState& state, int x, int y)
{
    constexpr UINT open = 1;
    constexpr UINT quit = 2;
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        restoreWindow(window, state);
        return;
    }
    if (!AppendMenuW(menu, MF_STRING, open, L"Ouvrir Plop-Wallpaper") ||
        !AppendMenuW(menu, MF_STRING, quit, L"Quitter")) {
        DestroyMenu(menu);
        restoreWindow(window, state);
        return;
    }
    SetForegroundWindow(window);
    const UINT command = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                        x, y, 0, window, nullptr);
    DestroyMenu(menu);
    PostMessageW(window, WM_NULL, 0, 0);
    if (command == open) {
        restoreWindow(window, state);
    } else if (command == quit) {
        DestroyWindow(window);
    } else {
        auto icon = trayIcon(window);
        Shell_NotifyIconW(NIM_SETFOCUS, &icon);
    }
}

void showRotationSettings(HWND window, const RotationSettings& rotation)
{
    CheckDlgButton(window, randomId, rotation.random ? BST_CHECKED : BST_UNCHECKED);
    SetDlgItemInt(window, minutesId, rotation.minutes, FALSE);
}

constexpr PCWSTR fitLabels[]{L"Étirer", L"Ajuster à la largeur", L"Ajuster à la hauteur"};

int monitorFit(const WindowState& state, size_t index)
{
    RECT bounds{};
    if (state.wallpaper->GetMonitorRECT(state.monitors[index].id.c_str(), &bounds) != S_OK ||
        bounds.bottom - bounds.top <= bounds.right - bounds.left) {
        return 0;
    }
    return state.selections[index].fit == PortraitFit::Width ? 1 : 2;
}

bool updateMonitorRow(HWND window, const WindowState& state, int row)
{
    if (row >= ListView_GetItemCount(GetDlgItem(window, monitorListId))) {
        return false;
    }
    const auto index = static_cast<size_t>(row);
    const auto& selection = state.selections[index];
    PCWSTR cells[]{
        selection.rotationMode ? L"Rotation" : L"Unique",
        selection.rotationMode && state.rotation.random ? L"Aléatoire" : L"Fixe",
        selection.folder.empty() ? L"—" : selection.folder.c_str(),
        fitLabels[monitorFit(state, index)]
    };
    const HWND list = GetDlgItem(window, monitorListId);
    for (int column = 0; column < ARRAYSIZE(cells); ++column) {
        LVITEMW item{};
        item.iSubItem = column + 1;
        item.pszText = const_cast<PWSTR>(cells[column]);
        if (!SendMessageW(list, LVM_SETITEMTEXTW, row, reinterpret_cast<LPARAM>(&item))) {
            logError(L"Impossible d’actualiser les réglages dans le tableau des écrans.");
            return false;
        }
    }
    return true;
}

bool hasRotatingMonitors(const WindowState& state)
{
    return std::any_of(state.selections.begin(), state.selections.end(),
                       [](const MonitorSettings& monitor) { return monitor.rotates(); });
}

HRESULT scheduleRotation(HWND window, WindowState& state, UINT minutes, bool restart = false)
{
    const bool active = hasRotatingMonitors(state);
    if (!active) {
        KillTimer(window, rotationTimer);
        state.rotationScheduled = false;
        return S_OK;
    }
    if (state.rotationScheduled && !restart) {
        return S_OK;
    }
    SetLastError(ERROR_SUCCESS);
    if (!SetTimer(window, rotationTimer, minutes * 60000, nullptr)) {
        const auto error = GetLastError();
        return error ? HRESULT_FROM_WIN32(error) : E_FAIL;
    }
    state.rotationScheduled = true;
    return S_OK;
}

void syncRotation(HWND window, WindowState& state)
{
    if (FAILED(scheduleRotation(window, state, state.rotation.minutes))) {
        logError(L"Impossible de programmer la rotation.");
        SetDlgItemTextW(window, statusId, L"Impossible de programmer la rotation.");
    }
}

void applyRotationSettings(HWND window, WindowState& state)
{
    BOOL valid = FALSE;
    const UINT minutes = GetDlgItemInt(window, minutesId, &valid, FALSE);
    if (!valid || minutes < 1 || minutes > RotationSettings::maxMinutes) {
        SetDlgItemTextW(window, statusId, L"Saisissez un intervalle de 1 à 35 791 minutes.");
        SetFocus(GetDlgItem(window, minutesId));
        return;
    }
    const RotationSettings next{minutes, IsDlgButtonChecked(window, randomId) == BST_CHECKED};
    HRESULT result = scheduleRotation(window, state, next.minutes, true);
    if (SUCCEEDED(result)) {
        result = state.settings.saveRotation(next);
        if (FAILED(result) && FAILED(scheduleRotation(window, state, state.rotation.minutes, true))) {
            KillTimer(window, rotationTimer);
            state.rotationScheduled = false;
            logError(L"Impossible de rétablir le minuteur de rotation.");
        }
    }
    if (FAILED(result)) {
        showRotationSettings(window, state.rotation);
        showError(window, L"Impossible d’enregistrer ou de programmer la rotation.", result);
        return;
    }
    state.rotation = next;
    const int rows = ListView_GetItemCount(GetDlgItem(window, monitorListId));
    for (int row = 0; row < rows; ++row) {
        updateMonitorRow(window, state, row);
    }
    SetDlgItemTextW(window, statusId, L"Réglages de rotation enregistrés.");
}

HRESULT advanceWallpaper(WindowState& state, size_t index)
{
    auto& selection = state.selections[index];
    const auto& monitor = state.monitors[index];
    const HRESULT result = changeWallpaper(*state.wallpaper.Get(), *state.imaging.Get(),
        monitor, selection.fit, state.cache, selection.folder, selection.lastImage,
        selection.currentImage(), state.rotation.random ? &state.rotationRandom : nullptr);
    if (result != S_OK) {
        return result;
    }
    selection.previousFolderImage.clear();
    selection.uniqueActive = false;
    return state.settings.saveMonitor(monitor, selection);
}

void rotateWallpapers(HWND window, WindowState& state)
{
    bool failed = false;
    for (size_t index = 0; index < state.selections.size(); ++index) {
        const auto& selection = state.selections[index];
        if (selection.rotates()) {
            if (advanceWallpaper(state, index) != S_OK) {
                logError(L"Rotation : impossible de changer ou d’enregistrer un fond.");
                failed = true;
            }
            updateMonitorRow(window, state, static_cast<int>(index));
        }
    }
    if (failed) {
        SetDlgItemTextW(window, statusId, L"La rotation a rencontré une erreur sur au moins un écran.");
    }
}

void updateSelection(HWND window, WindowState& state, int index)
{
    const auto* selection = index < 0 || static_cast<size_t>(index) >= state.selections.size()
        ? nullptr : &state.selections[static_cast<size_t>(index)];
    const bool hasFolder = selection && !selection->folder.empty();
    SetDlgItemTextW(window, folderPathId, hasFolder ? selection->folder.c_str() : L"Aucun dossier sélectionné");
    SetDlgItemTextW(window, imagePathId, selection && !selection->uniqueImage.empty()
        ? selection->uniqueImage.c_str() : L"Aucune image sélectionnée");
    SetDlgItemTextW(window, statusId, L"");
    EnableWindow(GetDlgItem(window, chooseFolderId), selection != nullptr);
    EnableWindow(GetDlgItem(window, chooseImageId), selection != nullptr);
    EnableWindow(GetDlgItem(window, changeWallpaperId), hasFolder);
    const bool rotation = selection && selection->rotationMode;
    const bool unique = selection && !selection->rotationMode;
    CheckDlgButton(window, rotationModeId, rotation ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(window, uniqueModeId, unique ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(window, rotationModeId), selection != nullptr);
    EnableWindow(GetDlgItem(window, uniqueModeId), selection != nullptr);
    for (int id : {rotationSectionId, folderPathId, chooseFolderId, changeWallpaperId,
                   randomId, intervalLabelId, minutesId, applyRotationId}) {
        ShowWindow(GetDlgItem(window, id), rotation ? SW_SHOWNA : SW_HIDE);
    }
    for (int id : {uniqueSectionId, imagePathId, chooseImageId}) {
        ShowWindow(GetDlgItem(window, id), unique ? SW_SHOWNA : SW_HIDE);
    }
    const int fit = selection ? monitorFit(state, static_cast<size_t>(index)) : -1;
    EnableWindow(GetDlgItem(window, fitId), fit > 0);
    SendDlgItemMessageW(window, fitId, CB_SETCURSEL, fit, 0);
}

bool showMonitors(HWND window, WindowState& state, const std::wstring& selectedId)
{
    const HWND list = GetDlgItem(window, monitorListId);
    ListView_DeleteAllItems(list);
    int selected = -1;
    int row = 0;
    for (const auto& monitor : state.monitors) {
        LVITEMW item{};
        item.mask = LVIF_TEXT;
        item.iItem = row;
        item.pszText = const_cast<PWSTR>(monitor.label.c_str());
        if (ListView_InsertItem(list, &item) < 0 || !updateMonitorRow(window, state, row)) {
            ListView_DeleteAllItems(list);
            updateSelection(window, state, -1);
            return false;
        }
        if (selected < 0 || monitor.id == selectedId) {
            selected = row;
        }
        ++row;
    }
    SetDlgItemTextW(window, monitorLabelId,
        state.monitors.empty() ? L"Aucun écran détecté." : L"Écrans connectés");
    if (selected >= 0) {
        ListView_SetItemState(list, selected, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        ListView_EnsureVisible(list, selected, FALSE);
    }
    updateSelection(window, state, selected);
    return true;
}

bool refreshMonitors(HWND window, WindowState& state)
{
    std::vector<Monitor> monitors;
    const HRESULT result = readMonitors(*state.wallpaper.Get(), monitors);
    if (FAILED(result)) {
        wchar_t message[96]{};
        swprintf_s(message, L"Impossible d’actualiser les écrans connectés. (0x%08lX)",
                   static_cast<unsigned long>(result));
        logError(message);
        SetDlgItemTextW(window, statusId, message);
        return false;
    }
    state.monitorRetry.reset();

    const int index = ListView_GetNextItem(GetDlgItem(window, monitorListId), -1, LVNI_SELECTED);
    const auto selectedId = index < 0 ? std::wstring{} : state.monitors[static_cast<size_t>(index)].id;
    std::vector<MonitorSettings> selections;
    selections.reserve(monitors.size());
    for (const auto& monitor : monitors) {
        size_t previous = 0;
        while (previous < state.monitors.size() && state.monitors[previous].id != monitor.id) {
            ++previous;
        }
        selections.push_back(previous < state.monitors.size()
            ? std::move(state.selections[previous]) : state.settings.loadMonitor(monitor));
    }
    state.monitors = std::move(monitors);
    state.selections = std::move(selections);
    if (!showMonitors(window, state, selectedId)) {
        logError(L"Impossible d’afficher la liste des écrans.");
        SetDlgItemTextW(window, statusId, L"Impossible d’afficher la liste des écrans.");
    }
    syncRotation(window, state);
    return true;
}

void scheduleMonitorRefresh(HWND window, WindowState& state, UINT delay = monitorRefreshDelay)
{
    state.monitorsPending = true;
    if (!SetTimer(window, monitorTimer, delay, nullptr)) {
        logError(L"Impossible de programmer l’actualisation des écrans.");
        if (!state.handlingCommand) {
            state.monitorsPending = !refreshMonitors(window, state);
        }
    }
}

void activateTrayClick(HWND window, WindowState& state)
{
    if (state.handlingCommand || (state.monitorsPending && !refreshMonitors(window, state))) {
        return;
    }
    state.monitorsPending = false;
    if (!hasRotatingMonitors(state)) {
        restoreWindow(window, state);
    } else {
        rotateWallpapers(window, state);
    }
}

void handleCommand(HWND window, WindowState& state, WPARAM command)
{
    const int id = LOWORD(command);
    const int notification = HIWORD(command);
    if (id == closeToTrayId && notification == BN_CLICKED) {
        const bool enabled = IsDlgButtonChecked(window, closeToTrayId) == BST_CHECKED;
        const HRESULT result = state.settings.saveCloseToTray(enabled);
        if (FAILED(result)) {
            CheckDlgButton(window, closeToTrayId, state.closeToTray ? BST_CHECKED : BST_UNCHECKED);
            showError(window, L"Impossible d’enregistrer le réglage de fermeture.", result);
        } else {
            state.closeToTray = enabled;
            SetDlgItemTextW(window, statusId, L"Réglage de fermeture enregistré.");
        }
        return;
    }
    if (id == startupId && notification == BN_CLICKED) {
        const bool enabled = IsDlgButtonChecked(window, startupId) == BST_CHECKED;
        const HRESULT result = setStartupEnabled(state.executable, enabled);
        if (FAILED(result)) {
            CheckDlgButton(window, startupId, enabled ? BST_UNCHECKED : BST_CHECKED);
            showError(window, L"Impossible de modifier le lancement avec Windows.", result);
        } else {
            SetDlgItemTextW(window, statusId, L"Réglage de démarrage enregistré.");
        }
        return;
    }
    if (id == applyRotationId && notification == BN_CLICKED) {
        applyRotationSettings(window, state);
        return;
    }
    const int index = ListView_GetNextItem(GetDlgItem(window, monitorListId), -1, LVNI_SELECTED);
    if (index < 0) {
        updateSelection(window, state, -1);
        return;
    }
    auto& selection = state.selections[static_cast<size_t>(index)];
    const auto& monitor = state.monitors[static_cast<size_t>(index)];
    if (id == fitId && notification == CBN_SELCHANGE) {
        const auto choice = SendDlgItemMessageW(window, fitId, CB_GETCURSEL, 0, 0);
        if (choice != 1 && choice != 2) {
            updateSelection(window, state, index);
            return;
        }
        const auto fit = choice == 1 ? PortraitFit::Width : PortraitFit::Height;
        if (!selection.currentImage().empty() && fit != selection.fit) {
            const HRESULT result = applyWallpaper(*state.wallpaper.Get(), *state.imaging.Get(),
                monitor, fit, state.cache, selection.currentImage());
            if (FAILED(result)) {
                updateSelection(window, state, index);
                showError(window, L"Impossible d’ajuster le fond de cet écran.", result);
                return;
            }
        }
        auto next = selection;
        next.fit = fit;
        const HRESULT result = state.settings.saveMonitor(monitor, next);
        if (FAILED(result)) {
            if (!selection.currentImage().empty() && fit != selection.fit &&
                FAILED(applyWallpaper(*state.wallpaper.Get(), *state.imaging.Get(), monitor,
                                      selection.fit, state.cache, selection.currentImage()))) {
                logError(L"Impossible de rétablir l’ajustement précédent.");
            }
            updateSelection(window, state, index);
            showError(window, L"Impossible d’enregistrer l’ajustement.", result);
            return;
        }
        selection = std::move(next);
    } else if ((id == rotationModeId || id == uniqueModeId) && notification == BN_CLICKED) {
        auto next = selection;
        next.rotationMode = id == rotationModeId;
        const HRESULT result = state.settings.saveMonitor(monitor, next);
        if (FAILED(result)) {
            updateSelection(window, state, index);
            showError(window, L"Impossible d’enregistrer le mode de cet écran.", result);
            return;
        }
        selection = std::move(next);
        updateSelection(window, state, index);
        syncRotation(window, state);
    } else if (id == chooseFolderId && notification == BN_CLICKED) {
        std::filesystem::path folder;
        const HRESULT result = pickFolder(window, folder);
        if (FAILED(result)) {
            showError(window, L"Impossible de sélectionner le dossier.", result);
        } else if (result == S_OK) {
            auto next = selection;
            next.changeFolder(std::move(folder));
            const HRESULT saved = state.settings.saveMonitor(monitor, next);
            if (FAILED(saved)) {
                showError(window, L"Impossible d’enregistrer le dossier.", saved);
                return;
            }
            selection = std::move(next);
            updateSelection(window, state, index);
            syncRotation(window, state);
        }
    } else if (id == chooseImageId && notification == BN_CLICKED) {
        std::filesystem::path image;
        HRESULT result = pickImage(window, image);
        if (result == S_FALSE) {
            return;
        }
        if (SUCCEEDED(result)) {
            result = applyWallpaper(*state.wallpaper.Get(), *state.imaging.Get(),
                                    monitor, selection.fit, state.cache, image);
        }
        if (FAILED(result)) {
            showError(window, L"Impossible d’appliquer l’image à cet écran.", result);
            return;
        }
        selection.uniqueImage = std::move(image);
        selection.uniqueActive = true;
        updateSelection(window, state, index);
        result = state.settings.saveMonitor(monitor, selection);
        if (FAILED(result)) {
            showError(window, L"Impossible d’enregistrer le choix de l’image.", result);
        } else {
            SetDlgItemTextW(window, statusId, L"Image appliquée à l’écran sélectionné.");
        }
    } else if (id == changeWallpaperId && notification == BN_CLICKED && !selection.folder.empty()) {
        SetDlgItemTextW(window, statusId, L"");
        const HRESULT result = advanceWallpaper(state, static_cast<size_t>(index));
        if (FAILED(result)) {
            showError(window, L"Impossible d’appliquer ou d’enregistrer le fond de cet écran.", result);
        } else if (result == S_FALSE) {
            SetDlgItemTextW(window, statusId, L"Aucune image JPG, JPEG, PNG ou BMP lisible dans le dossier.");
        } else {
            SetDlgItemTextW(window, statusId, L"Fond d’écran appliqué.");
        }
    } else {
        return;
    }
    updateMonitorRow(window, state, index);
}

LRESULT CALLBACK windowProcedure(HWND window, UINT message, WPARAM key, LPARAM data)
{
    auto* windowState = reinterpret_cast<WindowState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
    if (windowState && message == windowState->taskbarCreated && windowState->taskbarCreated) {
        if (windowState->inTray && !addTrayIcon(window)) {
            restoreWindow(window, *windowState);
        }
        return 0;
    }
    switch (message) {
    case WM_CREATE: {
        const auto* creation = reinterpret_cast<const CREATESTRUCTW*>(data);
        auto& state = *static_cast<WindowState*>(creation->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));
        state.taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
        HWND label = createControl(window, L"STATIC", L"Écrans connectés", 0, 16, 16, 540, 22, monitorLabelId);
        HWND list = createControl(window, WC_LISTVIEWW, L"", WS_BORDER | WS_TABSTOP |
            LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER, 16, 40, 540, 66, monitorListId);
        if (!label || !list ||
            !createControl(window, L"STATIC", L"Mode de l’écran", 0, 16, 119, 104, 22) ||
            !createControl(window, L"BUTTON", L"Rotation", WS_GROUP | WS_TABSTOP | BS_AUTORADIOBUTTON,
                124, 115, 100, 24, rotationModeId) ||
            !createControl(window, L"BUTTON", L"Unique", BS_AUTORADIOBUTTON,
                228, 115, 100, 24, uniqueModeId) ||
            !createControl(window, L"STATIC", L"Rotation", WS_GROUP | SS_OWNERDRAW,
                16, 148, 540, 114, rotationSectionId) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
                30, 171, 512, 24, folderPathId) ||
            !createControl(window, L"BUTTON", L"Choisir un dossier…", WS_TABSTOP,
                30, 201, 156, 26, chooseFolderId) ||
            !createControl(window, L"BUTTON", L"Image suivante", WS_TABSTOP,
                194, 201, 130, 26, changeWallpaperId) ||
            !createControl(window, L"BUTTON", L"Aléatoire (commun)",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 230, 175, 24, randomId) ||
            !createControl(window, L"STATIC", L"Intervalle commun (minutes)", 0,
                211, 234, 172, 20, intervalLabelId) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL,
                387, 230, 58, 24, minutesId) ||
            !createControl(window, L"BUTTON", L"Appliquer", WS_TABSTOP,
                456, 229, 86, 26, applyRotationId) ||
            !createControl(window, L"STATIC", L"Unique", SS_OWNERDRAW, 16, 148, 540, 65, uniqueSectionId) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
                30, 171, 350, 24, imagePathId) ||
            !createControl(window, L"BUTTON", L"Choisir une image…", WS_TABSTOP,
                389, 170, 153, 26, chooseImageId) ||
            !createControl(window, L"STATIC", L"Générale", SS_OWNERDRAW, 16, 271, 540, 112) ||
            !createControl(window, L"STATIC", L"Ajustement du fond", 0, 30, 300, 180, 20) ||
            !createControl(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST,
                210, 295, 235, 120, fitId) ||
            !createControl(window, L"BUTTON", L"Lancer Plop-Wallpaper à l’ouverture de session",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 327, 512, 24, startupId) ||
            !createControl(window, L"BUTTON", L"Fermer dans la barre de notification",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 351, 512, 24, closeToTrayId) ||
            !createControl(window, L"STATIC", L"", 0, 16, 394, 540, 38, statusId)) {
            SetLastError(ERROR_CANNOT_MAKE);
            return -1;
        }
        ListView_SetExtendedListViewStyle(list, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER | LVS_EX_INFOTIP);
        constexpr PCWSTR headers[]{L"Nom écran", L"Mode actuel", L"Aléatoire/Fixe", L"Dossier", L"Ajustement"};
        constexpr int widths[]{100, 80, 94, 116, 130};
        for (int index = 0; index < ARRAYSIZE(headers); ++index) {
            LVCOLUMNW column{};
            column.mask = LVCF_TEXT | LVCF_WIDTH;
            column.pszText = const_cast<PWSTR>(headers[index]);
            column.cx = widths[index];
            if (ListView_InsertColumn(list, index, &column) < 0) {
                SetLastError(ERROR_CANNOT_MAKE);
                return -1;
            }
        }
        const HWND fit = GetDlgItem(window, fitId);
        for (PCWSTR mode : fitLabels) {
            if (SendMessageW(fit, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(mode)) < 0) {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return -1;
            }
        }
        if (!showMonitors(window, state, {})) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return -1;
        }
        SendDlgItemMessageW(window, minutesId, EM_SETLIMITTEXT, 5, 0);
        showRotationSettings(window, state.rotation);
        state.theme.refresh(window);
        return 0;
    }
    case WM_DISPLAYCHANGE:
        if (windowState) {
            scheduleMonitorRefresh(window, *windowState);
        }
        return 0;
    case WM_DEVICECHANGE:
        if (windowState && key == DBT_DEVNODES_CHANGED) {
            scheduleMonitorRefresh(window, *windowState);
            return TRUE;
        }
        break;
    case WM_SETTINGCHANGE:
    case WM_SYSCOLORCHANGE:
    case WM_THEMECHANGED:
        if (windowState) {
            windowState->theme.refresh(window);
        }
        break;
    case WM_ERASEBKGND:
        if (windowState) {
            RECT bounds{};
            GetClientRect(window, &bounds);
            windowState->theme.paintBackground(reinterpret_cast<HDC>(key), bounds);
            return TRUE;
        }
        break;
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
        if (windowState) {
            const auto brush = windowState->theme.colorControl(
                reinterpret_cast<HDC>(key), reinterpret_cast<HWND>(data), message);
            if (brush) {
                return reinterpret_cast<LRESULT>(brush);
            }
        }
        break;
    case WM_DRAWITEM:
        if (windowState) {
            const auto& item = *reinterpret_cast<const DRAWITEMSTRUCT*>(data);
            if (windowState->theme.drawSection(item) || windowState->theme.drawButton(item)) {
                return TRUE;
            }
        }
        break;
    case WM_NOTIFY: {
        const auto& notification = *reinterpret_cast<const NMHDR*>(data);
        if (windowState && notification.code == NM_CUSTOMDRAW &&
            (notification.idFrom == rotationModeId || notification.idFrom == uniqueModeId)) {
            const auto& draw = *reinterpret_cast<const NMCUSTOMDRAW*>(data);
            if (draw.dwDrawStage == CDDS_PREPAINT) {
                return windowState->theme.drawModeOption(notification.hwndFrom, draw.hdc, draw.rc);
            }
        }
        if (windowState && notification.idFrom == monitorListId && notification.code == LVN_ITEMCHANGED) {
            const auto& change = *reinterpret_cast<const NMLISTVIEW*>(data);
            if (((change.uOldState ^ change.uNewState) & LVIS_SELECTED) != 0) {
                const int index = ListView_GetNextItem(notification.hwndFrom, -1, LVNI_SELECTED);
                updateSelection(window, *windowState, index);
            }
        }
        break;
    }
    case WM_CLOSE:
        if (windowState && windowState->closeToTray) {
            hideToTray(window, *windowState);
            if (!windowState->inTray) {
                showError(window, L"Impossible de réduire dans la barre de notification.", E_FAIL);
            }
            return 0;
        }
        break;
    case WM_SIZE:
        if (key == SIZE_MINIMIZED && windowState) {
            hideToTray(window, *windowState);
        }
        break;
    case trayMessage:
        if (windowState && windowState->inTray && HIWORD(data) == trayIconId) {
            const UINT event = LOWORD(data);
            if (event == WM_CONTEXTMENU) {
                showTrayMenu(window, *windowState, GET_X_LPARAM(key), GET_Y_LPARAM(key));
            } else if (event == WM_LBUTTONDBLCLK) {
                restoreWindow(window, *windowState);
            } else if (event == NIN_KEYSELECT) {
                activateTrayClick(window, *windowState);
            } else if (event == WM_LBUTTONUP) {
                // A double-click notification must cancel the deferred single-click action.
                if (SetTimer(window, trayClickTimer, GetDoubleClickTime(), nullptr)) {
                    windowState->trayClick.defer();
                } else {
                    logError(L"Impossible de différer le clic sur l’icône de notification.");
                    restoreWindow(window, *windowState);
                }
            }
        }
        return 0;
    case WM_TIMER:
        if (key == trayClickTimer && windowState) {
            KillTimer(window, trayClickTimer);
            if (windowState->trayClick.take()) {
                activateTrayClick(window, *windowState);
            }
        } else if (key == monitorTimer && windowState && !windowState->handlingCommand) {
            KillTimer(window, monitorTimer);
            windowState->monitorsPending = !refreshMonitors(window, *windowState);
            if (windowState->monitorsPending && windowState->monitorRetry.take()) {
                scheduleMonitorRefresh(window, *windowState, monitorRetryDelay);
            }
        } else if (key == rotationTimer && windowState && windowState->rotationScheduled &&
                   !windowState->handlingCommand &&
                   (!windowState->monitorsPending || refreshMonitors(window, *windowState))) {
            windowState->monitorsPending = false;
            rotateWallpapers(window, *windowState);
        }
        return 0;
    case WM_COMMAND: {
        auto* state = reinterpret_cast<WindowState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (state && !state->handlingCommand) {
            // File dialogs dispatch messages while handleCommand holds references to the selection.
            state->handlingCommand = true;
            handleCommand(window, *state, key);
            state->handlingCommand = false;
            if (state->monitorsPending) {
                scheduleMonitorRefresh(window, *state);
            }
        }
        return 0;
    }
    case WM_DESTROY:
        if (windowState && windowState->inTray) {
            auto icon = trayIcon(window);
            Shell_NotifyIconW(NIM_DELETE, &icon);
        }
        KillTimer(window, rotationTimer);
        KillTimer(window, monitorTimer);
        KillTimer(window, trayClickTimer);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, key, data);
}

int run(HINSTANCE instance, int show)
{
    INITCOMMONCONTROLSEX controls{sizeof(controls), ICC_LISTVIEW_CLASSES};
    if (!InitCommonControlsEx(&controls)) {
        showError(nullptr, L"Impossible d’initialiser le tableau des écrans.", E_FAIL);
        return 1;
    }
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (!length || length == executable.size()) {
        showError(nullptr, L"Impossible de localiser l’application.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }
    executable.resize(length);
    WindowState state{std::filesystem::path(executable)};
    HRESULT result = CoCreateInstance(CLSID_DesktopWallpaper, nullptr, CLSCTX_ALL,
                                      IID_PPV_ARGS(state.wallpaper.GetAddressOf()));
    if (FAILED(result)) {
        showError(nullptr, L"Impossible d’accéder aux fonds d’écran Windows.", result);
        return 1;
    }
    state.rotation = state.settings.loadRotation();
    state.closeToTray = state.settings.loadCloseToTray();
    result = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(state.imaging.GetAddressOf()));
    if (FAILED(result)) {
        showError(nullptr, L"Impossible d’initialiser la lecture des images.", result);
        return 1;
    }

    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = windowProcedure;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hIcon = LoadIconW(instance, MAKEINTRESOURCEW(IDI_APP_ICON));
    windowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    windowClass.lpszClassName = appName;
    if (!RegisterClassW(&windowClass)) {
        showError(nullptr, L"Impossible d’enregistrer la fenêtre.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }

    constexpr DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT size{0, 0, 572, 439};
    AdjustWindowRect(&size, style, FALSE);
    HWND window = CreateWindowW(appName, appName, style, CW_USEDEFAULT, CW_USEDEFAULT,
                                size.right - size.left, size.bottom - size.top,
                                nullptr, nullptr, instance, &state);
    if (!window) {
        showError(nullptr, L"Impossible d’ouvrir la fenêtre.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }
    // Keep the window available for display notifications if Windows cannot enumerate monitors yet.
    if (!refreshMonitors(window, state)) {
        scheduleMonitorRefresh(window, state);
    }
    hideToTray(window, state);
    if (!state.inTray) {
        ShowWindow(window, show);
    }
    CheckDlgButton(window, closeToTrayId, state.closeToTray ? BST_CHECKED : BST_UNCHECKED);
    bool startupEnabled = false;
    result = readStartupEnabled(startupEnabled);
    if (FAILED(result)) {
        EnableWindow(GetDlgItem(window, startupId), FALSE);
        showError(window, L"Impossible de lire le réglage de démarrage.", result);
    } else {
        CheckDlgButton(window, startupId, startupEnabled ? BST_CHECKED : BST_UNCHECKED);
    }
    MSG message{};
    BOOL received;
    while ((received = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        if (!IsDialogMessageW(window, &message)) {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    if (received < 0) {
        showError(window, L"Impossible de lire les messages de la fenêtre.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }
    return static_cast<int>(message.wParam);
}

}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int show)
{
    SetLastError(ERROR_SUCCESS);
    const HANDLE mutex = CreateMutexExW(nullptr, L"Local\\Plop-Wallpaper.SingleInstance", 0, SYNCHRONIZE);
    const DWORD mutexError = GetLastError();
    // Keep the named object alive until application shutdown.
    const std::unique_ptr<void, decltype(&CloseHandle)> instanceGuard(mutex, CloseHandle);
    if (!instanceGuard) {
        showError(nullptr, L"Impossible de vérifier si Plop-Wallpaper est déjà lancé.",
                  HRESULT_FROM_WIN32(mutexError));
        return 1;
    }
    if (mutexError == ERROR_ALREADY_EXISTS) {
        return 0;
    }

    const HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(result)) {
        showError(nullptr, L"Impossible d’initialiser les composants Windows.", result);
        return 1;
    }
    const int exitCode = run(instance, show);
    CoUninitialize();
    return exitCode;
}
