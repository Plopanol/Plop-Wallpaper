#pragma once

#include "monitors.h"

#include <filesystem>
#include <random>
#include <wincodec.h>

enum class PortraitFit { Width, Height };

HRESULT pickFolder(HWND owner, std::filesystem::path& folder);
HRESULT pickImage(HWND owner, std::filesystem::path& image);
HRESULT changeWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                        const Monitor& monitor, PortraitFit fit,
                        const std::filesystem::path& cache, const std::filesystem::path& folder,
                        std::filesystem::path& lastImage, const std::filesystem::path& currentImage,
                        std::mt19937* random);
HRESULT applyWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                       const Monitor& monitor, PortraitFit fit,
                       const std::filesystem::path& cache, const std::filesystem::path& image);
