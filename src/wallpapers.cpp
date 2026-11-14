#include "wallpapers.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>
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

HRESULT renderImage(IWICImagingFactory& imaging, const fs::path& image, const fs::path& output,
                    LONG width, LONG height, PortraitFit fit)
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
    UINT sourceWidth{}, sourceHeight{};
    result = frame->GetSize(&sourceWidth, &sourceHeight);
    if (FAILED(result)) {
        return result;
    }
    const auto sourcePixels = 1ull * sourceWidth * sourceHeight;
    const auto canvasPixels = 1ull * width * height;
    if (!sourceWidth || !sourceHeight || sourcePixels > UINT_MAX / 4 || canvasPixels > UINT_MAX / 4) {
        return WINCODEC_ERR_IMAGESIZEOUTOFRANGE;
    }
    const auto pixelBytes = static_cast<UINT>(4 * sourcePixels);
    const auto canvasBytes = static_cast<UINT>(4 * canvasPixels);

    LONG drawWidth = width;
    LONG drawHeight = height;
    if (height > width) {
        const double scaled = fit == PortraitFit::Width
            ? static_cast<double>(width) * sourceHeight / sourceWidth
            : static_cast<double>(height) * sourceWidth / sourceHeight;
        if (scaled > LONG_MAX) {
            return WINCODEC_ERR_IMAGESIZEOUTOFRANGE;
        }
        const auto dimension = std::max(1L, static_cast<LONG>(std::lround(scaled)));
        if (fit == PortraitFit::Width) {
            drawHeight = dimension;
        } else {
            drawWidth = dimension;
        }
    }

    ComPtr<IWICFormatConverter> converter;
    result = imaging.CreateFormatConverter(converter.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }
    result = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom);
    if (FAILED(result)) {
        return result;
    }
    std::vector<BYTE> source;
    try {
        source.resize(pixelBytes);
    } catch (const std::bad_alloc&) {
        return E_OUTOFMEMORY;
    }
    result = converter->CopyPixels(nullptr, 4 * sourceWidth, pixelBytes, source.data());
    if (FAILED(result)) {
        return result;
    }

    BITMAPINFO canvas{};
    canvas.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    canvas.bmiHeader.biWidth = width;
    canvas.bmiHeader.biHeight = -height;
    canvas.bmiHeader.biPlanes = 1;
    canvas.bmiHeader.biBitCount = 32;
    BITMAPINFO input = canvas;
    input.bmiHeader.biWidth = static_cast<LONG>(sourceWidth);
    input.bmiHeader.biHeight = -static_cast<LONG>(sourceHeight);
    const auto deleteContext = [](HDC context) { DeleteDC(context); };
    const auto deleteBitmap = [](HBITMAP bitmap) { DeleteObject(bitmap); };
    const std::unique_ptr<std::remove_pointer_t<HDC>, decltype(deleteContext)> context(
        CreateCompatibleDC(nullptr), deleteContext);
    void* pixels = nullptr;
    const std::unique_ptr<std::remove_pointer_t<HBITMAP>, decltype(deleteBitmap)> bitmap(
        CreateDIBSection(context.get(), &canvas, DIB_RGB_COLORS, &pixels, nullptr, 0), deleteBitmap);
    if (!context || !bitmap) {
        return E_OUTOFMEMORY;
    }
    std::memset(pixels, 0, static_cast<size_t>(canvasBytes));
    const HGDIOBJ previous = SelectObject(context.get(), bitmap.get());
    if (!previous || previous == HGDI_ERROR) {
        return E_FAIL;
    }
    SetStretchBltMode(context.get(), HALFTONE);
    SetBrushOrgEx(context.get(), 0, 0, nullptr);
    const int drawn = StretchDIBits(context.get(), (width - drawWidth) / 2, (height - drawHeight) / 2,
        drawWidth, drawHeight, 0, 0, static_cast<int>(sourceWidth), static_cast<int>(sourceHeight),
        source.data(), &input, DIB_RGB_COLORS, SRCCOPY);
    GdiFlush();
    SelectObject(context.get(), previous);
    if (drawn == GDI_ERROR || drawn == 0) {
        return E_FAIL;
    }

    ComPtr<IWICBitmap> rendered;
    result = imaging.CreateBitmapFromMemory(static_cast<UINT>(width), static_cast<UINT>(height),
        GUID_WICPixelFormat32bppBGR, static_cast<UINT>(4ull * width),
        canvasBytes, static_cast<BYTE*>(pixels), rendered.GetAddressOf());
    if (FAILED(result)) {
        return result;
    }
    ComPtr<IWICStream> stream;
    result = imaging.CreateStream(stream.GetAddressOf());
    if (SUCCEEDED(result)) {
        result = stream->InitializeFromFilename(output.c_str(), GENERIC_WRITE);
    }
    ComPtr<IWICBitmapEncoder> encoder;
    if (SUCCEEDED(result)) {
        result = imaging.CreateEncoder(GUID_ContainerFormatBmp, nullptr, encoder.GetAddressOf());
    }
    if (SUCCEEDED(result)) {
        result = encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache);
    }
    ComPtr<IWICBitmapFrameEncode> encodedFrame;
    if (SUCCEEDED(result)) {
        result = encoder->CreateNewFrame(encodedFrame.GetAddressOf(), nullptr);
    }
    if (SUCCEEDED(result)) {
        result = encodedFrame->Initialize(nullptr);
    }
    if (SUCCEEDED(result)) {
        result = encodedFrame->WriteSource(rendered.Get(), nullptr);
    }
    if (SUCCEEDED(result)) {
        result = encodedFrame->Commit();
    }
    return SUCCEEDED(result) ? encoder->Commit() : result;
}

void removeCachedImage(const fs::path& path)
{
    std::error_code error;
    fs::remove(path, error);
    if (error) {
        OutputDebugStringW(L"Impossible de supprimer un ancien fond généré.\n");
    }
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

HRESULT applyWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                       const Monitor& monitor, PortraitFit fit, const fs::path& cache,
                       const fs::path& image)
{
    RECT bounds{};
    const HRESULT rectResult = wallpaper.GetMonitorRECT(monitor.id.c_str(), &bounds);
    if (FAILED(rectResult)) {
        return rectResult;
    }
    if (rectResult != S_OK || IsRectEmpty(&bounds)) {
        return HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED);
    }
    std::error_code error;
    fs::create_directories(cache, error);
    if (error) {
        return HRESULT_FROM_WIN32(error.value());
    }
    GUID id{};
    HRESULT result = CoCreateGuid(&id);
    if (FAILED(result)) {
        return result;
    }
    wchar_t filename[39]{};
    StringFromGUID2(id, filename, ARRAYSIZE(filename));
    const auto output = cache / (std::wstring(filename) + L".bmp");
    result = renderImage(imaging, image, output, bounds.right - bounds.left, bounds.bottom - bounds.top, fit);
    if (FAILED(result)) {
        removeCachedImage(output);
        return result;
    }

    PWSTR rawPrevious = nullptr;
    if (FAILED(wallpaper.GetWallpaper(monitor.id.c_str(), &rawPrevious))) {
        OutputDebugStringW(L"Impossible de lire le fond précédent.\n");
    }
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> previous(rawPrevious, CoTaskMemFree);
    result = wallpaper.SetWallpaper(monitor.id.c_str(), output.c_str());
    if (result != S_OK) {
        removeCachedImage(output);
        return FAILED(result) ? result : E_FAIL;
    }
    if (previous) {
        const fs::path previousPath(previous.get());
        if (previousPath.parent_path() == cache && previousPath.extension() == L".bmp") {
            GUID previousId{};
            if (SUCCEEDED(CLSIDFromString(previousPath.stem().c_str(), &previousId))) {
                removeCachedImage(previousPath);
            }
        }
    }
    return S_OK;
}

HRESULT changeWallpaper(IDesktopWallpaper& wallpaper, IWICImagingFactory& imaging,
                        const Monitor& monitor, PortraitFit fit, const fs::path& cache,
                        const fs::path& folder, fs::path& lastImage)
{
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
    for (size_t tried = 0; tried < images.size(); ++tried, ++next) {
        if (next == images.end()) {
            next = images.begin();
        }
        const HRESULT result = applyWallpaper(wallpaper, imaging, monitor, fit, cache, *next);
        if (HRESULT_FACILITY(result) == HRESULT_FACILITY(WINCODEC_ERR_BADIMAGE) ||
            result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND)) {
            OutputDebugStringW(L"Catalogue : une image illisible a été ignorée.\n");
            continue;
        }
        if (result == S_OK) {
            lastImage = *next;
        }
        return result;
    }
    return S_FALSE;
}
