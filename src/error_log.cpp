#include "error_log.h"

#include <windows.h>

#include <fstream>
#include <memory>

namespace fs = std::filesystem;

namespace {

constexpr std::uintmax_t maxLogBytes = 1'000'000;

std::string logLine(std::wstring_view message)
{
    SYSTEMTIME now{};
    GetLocalTime(&now);
    wchar_t prefix[48]{};
    swprintf_s(prefix, L"%04d-%02d-%02d %02d:%02d:%02d [ERREUR] ", now.wYear, now.wMonth,
               now.wDay, now.wHour, now.wMinute, now.wSecond);
    std::wstring line = prefix;
    line.append(message);
    line.append(L"\r\n");
    const auto length = static_cast<int>(line.size());
    const int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, line.data(), length,
                                          nullptr, 0, nullptr, nullptr);
    std::string utf8(bytes, '\0');
    if (bytes == 0 || WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, line.data(), length,
                                         utf8.data(), bytes, nullptr, nullptr) == 0) {
        return {};
    }
    return utf8;
}

bool writeApplicationLog(std::wstring_view message)
{
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length == executable.size()) {
        return false;
    }
    executable.resize(length);

    const std::unique_ptr<void, decltype(&CloseHandle)> mutex(
        CreateMutexW(nullptr, FALSE, L"Local\\Plop-Wallpaper.ErrorLog"), CloseHandle);
    if (!mutex) {
        return false;
    }
    const DWORD wait = WaitForSingleObject(mutex.get(), 1000);
    if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
        return false;
    }
    const std::unique_ptr<void, decltype(&ReleaseMutex)> lock(mutex.get(), ReleaseMutex);
    return appendErrorLog(fs::path(executable).parent_path(), message);
}

}

bool appendErrorLog(const fs::path& directory, std::wstring_view message)
{
    const auto line = logLine(message);
    if (line.empty() || line.size() > maxLogBytes) {
        return false;
    }

    std::error_code error;
    fs::create_directories(directory, error);
    if (error) {
        return false;
    }

    const auto log = directory / L"erreurs.log";
    const auto size = fs::file_size(log, error);
    if (error && error != std::errc::no_such_file_or_directory) {
        return false;
    }
    if (!error && size > maxLogBytes - line.size()) {
        const auto archive = directory / L"erreurs.1.log";
        if (!MoveFileExW(log.c_str(), archive.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            return false;
        }
    }

    std::ofstream stream(log, std::ios::binary | std::ios::app);
    stream.write(line.data(), static_cast<std::streamsize>(line.size()));
    stream.close();
    return static_cast<bool>(stream);
}

void logError(const wchar_t* message) noexcept
{
    const DWORD lastError = GetLastError();
    OutputDebugStringW(message);
    OutputDebugStringW(L"\n");
    try {
        if (!writeApplicationLog(message)) {
            OutputDebugStringW(L"Impossible d’écrire dans le journal des erreurs.\n");
        }
    } catch (...) {
        OutputDebugStringW(L"Impossible d’écrire dans le journal des erreurs.\n");
    }
    SetLastError(lastError);
}
