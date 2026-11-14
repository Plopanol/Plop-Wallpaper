#pragma once

#include <windows.h>
#include <shobjidl.h>
#include <string>
#include <vector>

struct Monitor {
    std::wstring id;
    std::wstring label;
};

HRESULT readMonitors(IDesktopWallpaper& wallpaper, std::vector<Monitor>& monitors);
