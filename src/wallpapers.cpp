#include "wallpapers.h"

#include <algorithm>
#include <memory>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

namespace {

bool isSupported(const fs::path& path)
{
    const auto extension = path.extension().native();
    for (PCWSTR supported : {L".jpg", L".jpeg", L".png", L".bmp"}) {
        if (CompareStringOrdinal(extension.c_str(), -1, supported, -1, TRUE) == CSTR_EQUAL) {
            return true;
        }
    }
    return false;
}

HRESULT checkImage(IWICImagingFactory& imaging, const fs::path& image)
{
    ComPtr<IWICBitmapDecoder> decoder;
    HRESULT result = imaging.CreateDecoderFromFilename(image.c_str(), nullptr, GENERIC_READ,
        WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }
    ComPtr<IWICBitmapFrameDecode> frame;
    result = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }
    ComPtr<IWICBitmap> bitmap;
    return imaging.CreateBitmapFromSource(frame.Get(), WICBitmapCacheOnLoad, bitmap.GetAddressOf());
}

}

HRESULT pickFolder(HWND owner, fs::path& folder)
{
    ComPtr<IFileOpenDialog> dialog;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
                                      IID_PPV_ARGS(dialog.GetAddressOf()));
    if (FAILED(result)) {
        return result;
    }
    FILEOPENDIALOGOPTIONS options{};
    result = dialog->GetOptions(&options);
    if (FAILED(result)) {
        return result;
    }
    result = dialog->SetOptions(options | FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);
    if (FAILED(result)) {
        return result;
    }
    result = dialog->Show(owner);
    if (result == HRESULT_FROM_WIN32(ERROR_CANCELLED)) {
        return S_FALSE;
    }
    if (FAILED(result)) {
        return result;
    }
    ComPtr<IShellItem> item;
    result = dialog->GetResult(item.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }
    PWSTR rawPath = nullptr;
    result = item->GetDisplayName(SIGDN_FILESYSPATH, &rawPath);
    std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> path(rawPath, CoTaskMemFree);
    if (SUCCEEDED(result)) {
        folder = path.get();
    }
    return result;
}

HRESULT changeWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                        const Monitor& monitor, const fs::path& folder, fs::path& lastImage)
{
    RECT bounds{};
    const HRESULT rectResult = wallpaper.GetMonitorRECT(monitor.id.c_str(), &bounds);
    if (FAILED(rectResult)) {
        return rectResult;
    }
    if (rectResult != S_OK || IsRectEmpty(&bounds)) {
        return HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }

    std::vector<fs::path> images;
    std::error_code error;
    fs::directory_iterator entry(folder, error);
    const fs::directory_iterator end;
    while (!error && entry != end) {
        std::error_code fileError;
        if (isSupported(entry->path()) && entry->is_regular_file(fileError)) {
            images.push_back(entry->path());
        }
        if (fileError) {
            OutputDebugStringW(L"Catalogue : impossible de lire les attributs d’une image.\n");
        }
        entry.increment(error);
    }
    if (error) {
        return HRESULT_FROM_WIN32(error.value());
    }
    if (images.empty()) {
        return S_FALSE;
    }

    std::ranges::sort(images);
    auto next = std::ranges::upper_bound(images, lastImage);
    HRESULT applyError = S_OK;
    for (size_t tried = 0; tried < images.size(); ++tried, ++next) {
        if (next == images.end()) {
            next = images.begin();
        }
        if (FAILED(checkImage(imaging, *next))) {
            OutputDebugStringW(L"Catalogue : une image illisible a été ignorée.\n");
            continue;
        }
        applyError = wallpaper.SetWallpaper(monitor.id.c_str(), next->c_str());
        if (applyError == S_OK) {
            lastImage = *next;
            return S_OK;
        }
        if (SUCCEEDED(applyError)) {
            applyError = E_FAIL;
        }
    }
    return FAILED(applyError) ? applyError : S_FALSE;
}
