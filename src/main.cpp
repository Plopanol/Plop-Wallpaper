#include "settings.h"

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
constexpr UINT_PTR rotationTimer = 1;

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
    explicit WindowState(const std::filesystem::path& directory)
        : cache(directory / L"fonds-générés"), settings(directory / L"config.ini")
    {
    }

    std::vector<Monitor> monitors;
    ComPtr<IDesktopWallpaper> wallpaper;
    ComPtr<IWICImagingFactory> imaging;
    std::filesystem::path cache;
    Settings settings;
    RotationSettings rotation;
    std::vector<MonitorSettings> selections;
};

void showRotationSettings(HWND window, const RotationSettings& rotation)
{
    CheckDlgButton(window, rotationId, rotation.enabled ? BST_CHECKED : BST_UNCHECKED);
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
    const RotationSettings next{minutes, IsDlgButtonChecked(window, rotationId) == BST_CHECKED};
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
        monitor, selection.fit, state.cache, selection.folder, selection.lastImage);
    return result == S_OK ? state.settings.saveMonitor(monitor, selection) : result;
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
    SetDlgItemTextW(window, statusId, L"");
    EnableWindow(GetDlgItem(window, chooseFolderId), selection != nullptr);
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

void handleCommand(HWND window, WindowState& state, WPARAM command)
{
    const int id = LOWORD(command);
    const int notification = HIWORD(command);
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
        if (!selection.lastImage.empty() && fit != selection.fit) {
            const HRESULT result = applyWallpaper(*state.wallpaper.Get(), *state.imaging.Get(),
                state.monitors[static_cast<size_t>(index)], fit, state.cache, selection.lastImage);
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
            if (!selection.lastImage.empty() && fit != selection.fit &&
                FAILED(applyWallpaper(*state.wallpaper.Get(), *state.imaging.Get(), monitor,
                                      selection.fit, state.cache, selection.lastImage))) {
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
            next.folder = std::move(folder);
            next.lastImage.clear();
            const HRESULT saved = state.settings.saveMonitor(monitor, next);
            if (FAILED(saved)) {
                showError(window, L"Impossible d’enregistrer le dossier.", saved);
                return;
            }
            selection = std::move(next);
            updateSelection(window, state, index);
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
    switch (message) {
    case WM_CREATE: {
        const auto* creation = reinterpret_cast<const CREATESTRUCTW*>(data);
        auto& state = *static_cast<WindowState*>(creation->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&state));
        HWND label = createControl(window, L"STATIC", L"Écrans connectés", 0, 16, 16, 540, 22);
        HWND list = createControl(window, L"LISTBOX", L"", WS_BORDER | WS_VSCROLL |
            WS_TABSTOP | LBS_NOINTEGRALHEIGHT | LBS_NOTIFY, 16, 42, 540, 100, monitorListId);
        HWND fit = createControl(window, L"COMBOBOX", L"", WS_TABSTOP | CBS_DROPDOWNLIST,
            206, 253, 236, 120, fitId);
        if (!label || !list || !fit ||
            !createControl(window, L"STATIC", L"Dossier de l’écran sélectionné", 0, 16, 154, 540, 20) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_READONLY | ES_AUTOHSCROLL,
                16, 176, 540, 24, folderPathId) ||
            !createControl(window, L"BUTTON", L"Choisir un dossier…", WS_TABSTOP,
                16, 212, 178, 30, chooseFolderId) ||
            !createControl(window, L"BUTTON", L"Changer le fond", WS_TABSTOP,
                206, 212, 160, 30, changeWallpaperId) ||
            !createControl(window, L"STATIC", L"Ajustement du fond", 0, 16, 257, 180, 24) ||
            !createControl(window, L"BUTTON", L"Faire participer cet écran à la rotation",
                WS_TABSTOP | BS_AUTOCHECKBOX, 16, 290, 380, 24, participationId) ||
            !createControl(window, L"BUTTON", L"Activer la rotation générale",
                WS_TABSTOP | BS_AUTOCHECKBOX, 16, 332, 250, 24, rotationId) ||
            !createControl(window, L"STATIC", L"Intervalle (minutes)", 0, 270, 336, 125, 20) ||
            !createControl(window, L"EDIT", L"", WS_BORDER | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL,
                398, 332, 64, 24, minutesId) ||
            !createControl(window, L"BUTTON", L"Appliquer", WS_TABSTOP,
                470, 330, 86, 28, applyRotationId) ||
            !createControl(window, L"STATIC", L"", 0, 16, 374, 540, 42, statusId)) {
            SetLastError(ERROR_CANNOT_MAKE);
            return -1;
        }
        for (PCWSTR mode : {L"Étirer", L"Ajuster à la largeur", L"Ajuster à la hauteur"}) {
            if (SendMessageW(fit, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(mode)) < 0) {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return -1;
            }
        }
        for (const auto& monitor : state.monitors) {
            if (SendMessageW(list, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(monitor.label.c_str())) < 0) {
                SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                return -1;
            }
        }
        if (state.monitors.empty()) {
            SetWindowTextW(label, L"Aucun écran détecté.");
            updateSelection(window, state, LB_ERR);
        } else {
            SendMessageW(list, LB_SETCURSEL, 0, 0);
            updateSelection(window, state, 0);
        }
        SendDlgItemMessageW(window, minutesId, EM_SETLIMITTEXT, 5, 0);
        showRotationSettings(window, state.rotation);
        return 0;
    }
    case WM_TIMER: {
        auto* state = reinterpret_cast<WindowState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (key == rotationTimer && state && state->rotation.enabled) {
            rotateWallpapers(window, *state);
        }
        return 0;
    }
    case WM_COMMAND: {
        auto* state = reinterpret_cast<WindowState*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (state) {
            handleCommand(window, *state, key);
        }
        return 0;
    }
    case WM_DESTROY:
        KillTimer(window, rotationTimer);
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProcW(window, message, key, data);
    }
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
    WindowState state(std::filesystem::path(executable).parent_path());
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
    windowClass.hIcon = LoadIconW(nullptr, IDI_APPLICATION);
    windowClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    windowClass.lpszClassName = appName;
    if (!RegisterClassW(&windowClass)) {
        showError(nullptr, L"Impossible d’enregistrer la fenêtre.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }

    constexpr DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT size{0, 0, 572, 430};
    AdjustWindowRect(&size, style, FALSE);
    HWND window = CreateWindowW(appName, appName, style, CW_USEDEFAULT, CW_USEDEFAULT,
                                size.right - size.left, size.bottom - size.top,
                                nullptr, nullptr, instance, &state);
    if (!window) {
        showError(nullptr, L"Impossible d’ouvrir la fenêtre.", HRESULT_FROM_WIN32(GetLastError()));
        return 1;
    }
    ShowWindow(window, show);
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
