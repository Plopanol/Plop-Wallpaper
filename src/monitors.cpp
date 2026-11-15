#include "monitors.h"

#include <algorithm>
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

std::vector<DISPLAYCONFIG_TARGET_DEVICE_NAME> readActiveTargets()
{
    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    if (FAILED(readActivePaths(paths))) {
        OutputDebugStringW(L"QueryDisplayConfig : noms des moniteurs indisponibles.\n");
        return {};
    }

    std::vector<DISPLAYCONFIG_TARGET_DEVICE_NAME> targets(paths.size());
    for (size_t index = 0; index < paths.size(); ++index) {
        const auto& path = paths[index];
        auto& target = targets[index];
        target.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        target.header.size = sizeof(target);
        target.header.adapterId = path.targetInfo.adapterId;
        target.header.id = path.targetInfo.id;
        const LONG result = DisplayConfigGetDeviceInfo(&target.header);
        if (result != ERROR_SUCCESS) {
            OutputDebugStringW(L"DisplayConfigGetDeviceInfo : lecture du nom impossible.\n");
            target.monitorDevicePath[0] = L'\0';
        }
    }
    return targets;
}

}

HRESULT readMonitors(IDesktopWallpaper& wallpaper, std::vector<Monitor>& monitors)
{
    return readMonitors(wallpaper, monitors, readActiveTargets());
}

HRESULT readMonitors(IDesktopWallpaper& wallpaper, std::vector<Monitor>& monitors,
                     std::span<const DISPLAYCONFIG_TARGET_DEVICE_NAME> activeTargets)
{
    const bool targetsKnown = !activeTargets.empty() &&
        std::ranges::all_of(activeTargets, [](const auto& target) { return target.monitorDevicePath[0] != L'\0'; });
    UINT count = 0;
    const HRESULT countResult = wallpaper.GetMonitorDevicePathCount(&count);
    if (FAILED(countResult)) {
        return countResult;
    }
    std::vector<Monitor> active;
    active.reserve(count);
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
        const auto target = std::ranges::find_if(activeTargets, [&id](const auto& candidate) {
            return CompareStringOrdinal(id.get(), -1, candidate.monitorDevicePath, -1, TRUE) == CSTR_EQUAL;
        });
        RECT bounds{};
        const HRESULT rectResult = wallpaper.GetMonitorRECT(id.get(), &bounds);
        if (FAILED(rectResult)) {
            // Windows can retain wallpaper entries for disconnected monitors and return E_FAIL.
            if (rectResult == E_FAIL && targetsKnown && target == activeTargets.end()) {
                continue;
            }
            return rectResult;
        }
        if (rectResult == S_FALSE || IsRectEmpty(&bounds)) {
            continue;
        }
        const LONG width = bounds.right - bounds.left;
        const LONG height = bounds.bottom - bounds.top;
        const wchar_t* orientation = width >= height ? L"Paysage" : L"Portrait";
        PCWSTR name = target != activeTargets.end() && target->monitorFriendlyDeviceName[0] != L'\0'
            ? target->monitorFriendlyDeviceName : L"Moniteur";
        const auto number = std::to_wstring(active.size() + 1);
        active.push_back({id.get(), number + L" — " + name + L" | " +
            std::to_wstring(width) + L" × " + std::to_wstring(height) + L" | " + orientation});
    }
    monitors = std::move(active);
    return S_OK;
}
