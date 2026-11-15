#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <span>
#include <string>
#include <vector>

struct Monitor {
    std::wstring id;
    std::wstring label;
};

HRESULT readMonitors(IDesktopWallpaper& wallpaper, std::vector<Monitor>& monitors);
HRESULT readMonitors(IDesktopWallpaper& wallpaper, std::vector<Monitor>& monitors,
                     std::span<const DISPLAYCONFIG_TARGET_DEVICE_NAME> activeTargets);
