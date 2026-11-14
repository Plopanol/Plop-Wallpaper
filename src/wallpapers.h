#pragma once

#include "monitors.h"

#include <filesystem>
#include <wincodec.h>

HRESULT pickFolder(HWND owner, std::filesystem::path& folder);
HRESULT changeWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                        const Monitor& monitor, const std::filesystem::path& folder,
                        std::filesystem::path& lastImage);
