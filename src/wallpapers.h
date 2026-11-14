#pragma once

#include "monitors.h"

#include <filesystem>
#include <wincodec.h>

enum class PortraitFit { Width, Height };

HRESULT pickFolder(HWND owner, std::filesystem::path& folder);
HRESULT changeWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                        const Monitor& monitor, PortraitFit fit,
                        const std::filesystem::path& cache, const std::filesystem::path& folder,
                        std::filesystem::path& lastImage);
HRESULT applyWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                       const Monitor& monitor, PortraitFit fit,
                       const std::filesystem::path& cache, const std::filesystem::path& image);
