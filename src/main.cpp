#include "resource.h"
#include "settings.h"
#include "startup.h"
#include "theme.h"

#include <dbt.h>
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
constexpr int participationId = 106;
constexpr int rotationId = 107;
constexpr int minutesId = 108;
constexpr int applyRotationId = 109;
constexpr int startupId = 110;
constexpr int chooseImageId = 111;
constexpr int imagePathId = 112;
constexpr int randomId = 113;
constexpr int monitorLabelId = 114;
constexpr UINT_PTR rotationTimer = 1;
constexpr UINT_PTR monitorTimer = 2;
constexpr UINT monitorRefreshDelay = 250;
constexpr UINT trayMessage = WM_APP + 1;
constexpr UINT trayIconId = 1;

void showError(HWND owner, PCWSTR message, HRESULT result)
{
    wchar_t code[16]{};
    swprintf_s(code, L"0x%08lX", static_cast<unsigned long>(result));
    std::wstring detail = std::wstring(message) + L" (" + code + L")";
    OutputDebugStringW(detail.c_str());
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
    bool handlingCommand = false;
    bool monitorsPending = false;
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
    OutputDebugStringW(L"Impossible d’ajouter l’icône de notification.\n");
    return false;
}

void restoreWindow(HWND window, WindowState& state)
{
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
    CheckDlgButton(window, rotationId, rotation.enabled ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(window, randomId, rotation.random ? BST_CHECKED : BST_UNCHECKED);
    SetDlgItemInt(window, minutesId, rotation.minutes, FALSE);
}

HRESULT scheduleRotation(HWND window, const RotationSettings& rotation)
{
    if (!rotation.enabled) {
        KillTimer(window, rotationTimer);
        return S_OK;
    }
    SetLastError(ERROR_SUCCESS);
    if (!SetTimer(window, rotationTimer, rotation.minutes * 60000, nullptr)) {
        const auto error = GetLastError();
        return error ? HRESULT_FROM_WIN32(error) : E_FAIL;
    }
    return S_OK;
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
    const RotationSettings next{minutes, IsDlgButtonChecked(window, rotationId) == BST_CHECKED,
        IsDlgButtonChecked(window, randomId) == BST_CHECKED};
    HRESULT result = scheduleRotation(window, next);
    if (SUCCEEDED(result)) {
        result = state.settings.saveRotation(next);
        if (FAILED(result) && FAILED(scheduleRotation(window, state.rotation))) {
            state.rotation.enabled = false;
            OutputDebugStringW(L"Impossible de rétablir le minuteur de rotation.\n");
        }
    }
    if (FAILED(result)) {
        showRotationSettings(window, state.rotation);
        showError(window, L"Impossible d’enregistrer ou de programmer la rotation.", result);
        return;
    }
    state.rotation = next;
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
        if (selection.participates && !selection.folder.empty()) {
            if (advanceWallpaper(state, index) != S_OK) {
                OutputDebugStringW(L"Rotation : impossible de changer ou d’enregistrer un fond.\n");
                failed = true;
            }
        }
    }
    if (failed) {
        SetDlgItemTextW(window, statusId, L"La rotation a rencontré une erreur sur au moins un écran.");
    }
}

void updateSelection(HWND window, WindowState& state, LRESULT index)
{
    const auto* selection = index == LB_ERR ? nullptr : &state.selections[static_cast<size_t>(index)];
    const bool hasFolder = selection && !selection->folder.empty();
    SetDlgItemTextW(window, folderPathId, hasFolder ? selection->folder.c_str() : L"Aucun dossier sélectionné");
    SetDlgItemTextW(window, imagePathId, selection && !selection->uniqueImage.empty()
        ? selection->uniqueImage.c_str() : L"Aucune image sélectionnée");
    SetDlgItemTextW(window, statusId, L"");
    EnableWindow(GetDlgItem(window, chooseFolderId), selection != nullptr);
    EnableWindow(GetDlgItem(window, chooseImageId), selection != nullptr);
    EnableWindow(GetDlgItem(window, changeWallpaperId), hasFolder);
    EnableWindow(GetDlgItem(window, participationId), selection != nullptr);
    CheckDlgButton(window, participationId, selection && selection->participates ? BST_CHECKED : BST_UNCHECKED);
    RECT bounds{};
    const bool portrait = selection &&
        state.wallpaper->GetMonitorRECT(state.monitors[static_cast<size_t>(index)].id.c_str(), &bounds) == S_OK &&
        bounds.bottom - bounds.top > bounds.right - bounds.left;
    EnableWindow(GetDlgItem(window, fitId), portrait);
    const int fit = portrait ? (selection->fit == PortraitFit::Width ? 1 : 2) : (selection ? 0 : -1);
    SendDlgItemMessageW(window, fitId, CB_SETCURSEL, fit, 0);
}

bool showMonitors(HWND window, WindowState& state, const std::wstring& selectedId)
{
    const HWND list = GetDlgItem(window, monitorListId);
    SendMessageW(list, LB_RESETCONTENT, 0, 0);
    LRESULT selected = LB_ERR;
    for (const auto& monitor : state.monitors) {
        const auto index = SendMessageW(list, LB_ADDSTRING, 0,
            reinterpret_cast<LPARAM>(monitor.label.c_str()));
        if (index < 0) {
            SendMessageW(list, LB_RESETCONTENT, 0, 0);
            updateSelection(window, state, LB_ERR);
            return false;
        }
        if (selected == LB_ERR || monitor.id == selectedId) {
            selected = index;
        }
    }
    SetDlgItemTextW(window, monitorLabelId,
        state.monitors.empty() ? L"Aucun écran détecté." : L"Écrans connectés");
    SendMessageW(list, LB_SETCURSEL, selected, 0);
    updateSelection(window, state, selected);
    return true;
}

bool refreshMonitors(HWND window, WindowState& state)
{
    std::vector<Monitor> monitors;
    if (FAILED(readMonitors(*state.wallpaper.Get(), monitors))) {
        OutputDebugStringW(L"Impossible d’actualiser les écrans connectés.\n");
        SetDlgItemTextW(window, statusId, L"Impossible d’actualiser les écrans connectés.");
        return false;
    }

    const auto index = SendDlgItemMessageW(window, monitorListId, LB_GETCURSEL, 0, 0);
    const auto selectedId = index == LB_ERR ? std::wstring{} : state.monitors[static_cast<size_t>(index)].id;
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
        OutputDebugStringW(L"Impossible d’afficher la liste des écrans.\n");
        SetDlgItemTextW(window, statusId, L"Impossible d’afficher la liste des écrans.");
    }
    return true;
}

void scheduleMonitorRefresh(HWND window, WindowState& state)
{
    state.monitorsPending = true;
    if (!SetTimer(window, monitorTimer, monitorRefreshDelay, nullptr)) {
        OutputDebugStringW(L"Impossible de programmer l’actualisation des écrans.\n");
        if (!state.handlingCommand) {
            state.monitorsPending = !refreshMonitors(window, state);
        }
    }
}

void handleCommand(HWND window, WindowState& state, WPARAM command)
{
    const int id = LOWORD(command);
    const int notification = HIWORD(command);
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
    const LRESULT index = SendDlgItemMessageW(window, monitorListId, LB_GETCURSEL, 0, 0);
    if (index == LB_ERR) {
        updateSelection(window, state, LB_ERR);
        return;
    }
    auto& selection = state.selections[static_cast<size_t>(index)];
    const auto& monitor = state.monitors[static_cast<size_t>(index)];
    if (id == monitorListId && notification == LBN_SELCHANGE) {
        updateSelection(window, state, index);
    } else if (id == fitId && notification == CBN_SELCHANGE) {
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
                OutputDebugStringW(L"Impossible de rétablir l’ajustement précédent.\n");
            }
            updateSelection(window, state, index);
            showError(window, L"Impossible d’enregistrer l’ajustement.", result);
            return;
        }
        selection = std::move(next);
    } else if (id == participationId && notification == BN_CLICKED) {
        auto next = selection;
        next.participates = IsDlgButtonChecked(window, participationId) == BST_CHECKED;
        const HRESULT result = state.settings.saveMonitor(monitor, next);
        if (FAILED(result)) {
            updateSelection(window, state, index);
            showError(window, L"Impossible d’enregistrer la participation de cet écran.", result);
            return;
        }
        selection = std::move(next);
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
    }
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
        HWND list = createControl(window, L"LISTBOX", L"", WS_BORDER | WS_VSCROLL |
            WS_TABSTOP | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 16, 40, 540, 60, monitorListId);
        if (!label || !list ||
            !createControl(window, L"STATIC", L"Rotation", SS_OWNERDRAW, 16, 112, 540, 176) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
                30, 136, 512, 24, folderPathId) ||
            !createControl(window, L"BUTTON", L"Choisir un dossier…", WS_TABSTOP,
                30, 168, 156, 26, chooseFolderId) ||
            !createControl(window, L"BUTTON", L"Image suivante", WS_TABSTOP,
                194, 168, 130, 26, changeWallpaperId) ||
            !createControl(window, L"BUTTON", L"Faire participer cet écran à la rotation",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 198, 380, 22, participationId) ||
            !createControl(window, L"BUTTON", L"Activer la rotation générale",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 226, 235, 24, rotationId) ||
            !createControl(window, L"BUTTON", L"Aléatoire",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 254, 235, 24, randomId) ||
            !createControl(window, L"STATIC", L"Intervalle (minutes)", 0, 270, 258, 115, 20) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL,
                387, 254, 58, 24, minutesId) ||
            !createControl(window, L"BUTTON", L"Appliquer", WS_TABSTOP,
                456, 253, 86, 26, applyRotationId) ||
            !createControl(window, L"STATIC", L"Unique", SS_OWNERDRAW, 16, 297, 540, 65) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
                30, 323, 350, 24, imagePathId) ||
            !createControl(window, L"BUTTON", L"Choisir une image…", WS_TABSTOP,
                389, 322, 153, 26, chooseImageId) ||
            !createControl(window, L"STATIC", L"Générale", SS_OWNERDRAW, 16, 371, 540, 88) ||
            !createControl(window, L"STATIC", L"Ajustement du fond", 0, 30, 400, 180, 20) ||
            !createControl(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST,
                210, 395, 235, 120, fitId) ||
            !createControl(window, L"BUTTON", L"Lancer Plop-Wallpaper à l’ouverture de session",
                WS_TABSTOP | BS_AUTOCHECKBOX, 30, 427, 512, 24, startupId) ||
            !createControl(window, L"STATIC", L"", 0, 16, 470, 540, 38, statusId)) {
            SetLastError(ERROR_CANNOT_MAKE);
            return -1;
        }
        const HWND fit = GetDlgItem(window, fitId);
        for (PCWSTR mode : {L"Étirer", L"Ajuster à la largeur", L"Ajuster à la hauteur"}) {
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
    case WM_SIZE:
        if (key == SIZE_MINIMIZED && windowState && !windowState->inTray &&
            windowState->taskbarCreated && addTrayIcon(window)) {
            windowState->inTray = true;
            ShowWindow(window, SW_HIDE);
        }
        break;
    case trayMessage:
        if (windowState && windowState->inTray && HIWORD(data) == trayIconId) {
            const UINT event = LOWORD(data);
            if (event == WM_CONTEXTMENU) {
                showTrayMenu(window, *windowState, GET_X_LPARAM(key), GET_Y_LPARAM(key));
            } else if (event == WM_LBUTTONUP || event == NIN_KEYSELECT) {
                if (!windowState->rotation.enabled) {
                    restoreWindow(window, *windowState);
                } else if (!windowState->handlingCommand &&
                           (!windowState->monitorsPending || refreshMonitors(window, *windowState))) {
                    windowState->monitorsPending = false;
                    rotateWallpapers(window, *windowState);
                }
            }
        }
        return 0;
    case WM_TIMER:
        if (key == monitorTimer && windowState && !windowState->handlingCommand) {
            KillTimer(window, monitorTimer);
            windowState->monitorsPending = !refreshMonitors(window, *windowState);
        } else if (key == rotationTimer && windowState && windowState->rotation.enabled &&
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
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, message, key, data);
}

int run(HINSTANCE instance, int show)
{
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
    result = readMonitors(*state.wallpaper.Get(), state.monitors);
    if (FAILED(result)) {
        showError(nullptr, L"Impossible de détecter les écrans.", result);
        return 1;
    }
    for (const auto& monitor : state.monitors) {
        state.selections.push_back(state.settings.loadMonitor(monitor));
    }
    state.rotation = state.settings.loadRotation();
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
    RECT size{0, 0, 572, 515};
    AdjustWindowRect(&size, style, FALSE);
    HWND window = CreateWindowW(appName, appName, style, CW_USEDEFAULT, CW_USEDEFAULT,
                                size.right - size.left, size.bottom - size.top,
                                nullptr, nullptr, instance, &state);
    if (!window) {
        showError(nullptr, L"Impossible d’ouvrir la fenêtre.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }
    ShowWindow(window, show);
    bool startupEnabled = false;
    result = readStartupEnabled(startupEnabled);
    if (FAILED(result)) {
        EnableWindow(GetDlgItem(window, startupId), FALSE);
        showError(window, L"Impossible de lire le réglage de démarrage.", result);
    } else {
        CheckDlgButton(window, startupId, startupEnabled ? BST_CHECKED : BST_UNCHECKED);
    }
    result = scheduleRotation(window, state.rotation);
    if (FAILED(result)) {
        state.rotation.enabled = false;
        showRotationSettings(window, state.rotation);
        showError(window, L"Impossible de programmer la rotation au démarrage.", result);
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
    const HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(result)) {
        showError(nullptr, L"Impossible d’initialiser les composants Windows.", result);
        return 1;
    }
    const int exitCode = run(instance, show);
    CoUninitialize();
    return exitCode;
}
