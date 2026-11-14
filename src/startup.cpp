#include "startup.h"

#include <memory>
#include <shlobj.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

namespace {

HRESULT startupShortcut(fs::path& shortcut)
{
    PWSTR rawDirectory = nullptr;
    const HRESULT result = SHGetKnownFolderPath(FOLDERID_Startup, 0, nullptr, &rawDirectory);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> directory(rawDirectory, CoTaskMemFree);
    if (SUCCEEDED(result)) {
        shortcut = fs::path(directory.get()) / L"Plop-Wallpaper.lnk";
    }
    return result;
}

}

HRESULT readStartupEnabled(bool& enabled)
{
    fs::path shortcut;
    const HRESULT result = startupShortcut(shortcut);
    if (FAILED(result)) {
        return result;
    }
    std::error_code error;
    enabled = fs::is_regular_file(shortcut, error);
    if (error && error != std::errc::no_such_file_or_directory) {
        return HRESULT_FROM_WIN32(error.value());
    }
    return S_OK;
}

HRESULT setStartupEnabled(const fs::path& executable, bool enabled)
{
    fs::path shortcut;
    HRESULT result = startupShortcut(shortcut);
    if (FAILED(result)) {
        return result;
    }
    if (!enabled) {
        if (DeleteFileW(shortcut.c_str())) {
            return S_OK;
        }
        const DWORD error = GetLastError();
        return error == ERROR_FILE_NOT_FOUND ? S_OK : HRESULT_FROM_WIN32(error);
    }

    ComPtr<IShellLinkW> link;
    result = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                              IID_PPV_ARGS(link.GetAddressOf()));
    if (SUCCEEDED(result)) {
        result = link->SetPath(executable.c_str());
    }
    if (SUCCEEDED(result)) {
        result = link->SetWorkingDirectory(executable.parent_path().c_str());
    }
    ComPtr<IPersistFile> file;
    if (SUCCEEDED(result)) {
        result = link.As(&file);
    }
    return SUCCEEDED(result) ? file->Save(shortcut.c_str(), TRUE) : result;
}
