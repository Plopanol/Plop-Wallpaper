#include "monitors.h"

#include <memory>

namespace {

HRESULT readActivePaths(std::vector<DISPLAYCONFIG_PATH_INFO>& paths)
{
    LONG result;
    do {
        UINT32 pathCount = 0;
        UINT32 modeCount = 0;
        result = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount, &modeCount);
        if (result != ERROR_SUCCESS) {
            return HRESULT_FROM_WIN32(result);
        }

        paths.resize(pathCount);
        std::vector<DISPLAYCONFIG_MODE_INFO> modes(modeCount);
        result = QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                                    &modeCount, modes.data(), nullptr);
        if (result == ERROR_SUCCESS) {
            paths.resize(pathCount);
        }
    } while (result == ERROR_INSUFFICIENT_BUFFER);
    return HRESULT_FROM_WIN32(result);
}

std::wstring monitorName(const std::vector<DISPLAYCONFIG_PATH_INFO>& paths, PCWSTR id)
{
    for (const auto& path : paths) {
        DISPLAYCONFIG_TARGET_DEVICE_NAME name{};
        name.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        name.header.size = sizeof(name);
        name.header.adapterId = path.targetInfo.adapterId;
        name.header.id = path.targetInfo.id;
        const LONG result = DisplayConfigGetDeviceInfo(&name.header);
        if (result != ERROR_SUCCESS) {
            OutputDebugStringW(L"DisplayConfigGetDeviceInfo : lecture du nom impossible.\n");
            continue;
        }
        if (CompareStringOrdinal(id, -1, name.monitorDevicePath, -1, TRUE) == CSTR_EQUAL) {
            return name.monitorFriendlyDeviceName;
        }
    }
    return {};
}

}

HRESULT readMonitors(IDesktopWallpaper& wallpaper, std::vector<Monitor>& monitors)
{
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    if (FAILED(readActivePaths(paths))) {
        OutputDebugStringW(L"QueryDisplayConfig : noms des moniteurs indisponibles.\n");
        paths.clear();
    }

    UINT count = 0;
    const HRESULT countResult = wallpaper.GetMonitorDevicePathCount(&count);
    if (FAILED(countResult)) {
        return countResult;
    }
    for (UINT index = 0; index < count; ++index) {
        PWSTR rawId = nullptr;
        const HRESULT idResult = wallpaper.GetMonitorDevicePathAt(index, &rawId);
        std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> id(rawId, CoTaskMemFree);
        if (FAILED(idResult)) {
            return idResult;
        }
        if (!id || !*id.get()) {
            return E_UNEXPECTED;
        }
        RECT bounds{};
        const HRESULT rectResult = wallpaper.GetMonitorRECT(id.get(), &bounds);
        if (FAILED(rectResult)) {
            return rectResult;
        }
        if (rectResult == S_FALSE || IsRectEmpty(&bounds)) {
            continue;
        }
        const LONG width = bounds.right - bounds.left;
        const LONG height = bounds.bottom - bounds.top;
        const wchar_t* orientation = width >= height ? L"Paysage" : L"Portrait";
        std::wstring name = monitorName(paths, id.get());
        if (name.empty()) {
            name = L"Moniteur";
        }
        const auto number = std::to_wstring(monitors.size() + 1);
        monitors.push_back({id.get(), number + L" — " + name + L" | " +
            std::to_wstring(width) + L" × " + std::to_wstring(height) + L" | " + orientation});
    }
    return S_OK;
}
