#include "settings.h"

namespace {

std::wstring monitorSection(const Monitor& monitor)
{
    std::wstring section = L"Monitor:";
    constexpr wchar_t digits[] = L"0123456789abcdef";
    for (const wchar_t character : monitor.id) {
        for (int shift : {12, 8, 4, 0}) {
            section += digits[(character >> shift) & 15];
        }
    }
    return section;
}

std::filesystem::path readPath(PCWSTR section, PCWSTR key, PCWSTR ini)
{
    std::wstring value(32768, L'\0');
    const DWORD length = GetPrivateProfileStringW(section, key, L"", value.data(),
                                                   static_cast<DWORD>(value.size()), ini);
    if (length >= value.size() - 1) {
        OutputDebugStringW(L"Un chemin dans la configuration est trop long.\n");
        return {};
    }
    value.resize(length);
    return value;
}

void appendField(std::wstring& fields, PCWSTR key, PCWSTR value)
{
    fields += key;
    fields += L'=';
    fields += value;
    fields += L'\0';
}

}

Settings::Settings(std::filesystem::path path) : path_(std::move(path))
{
}

MonitorSettings Settings::loadMonitor(const Monitor& monitor) const
{
    const auto section = monitorSection(monitor);
    MonitorSettings value;
    value.folder = readPath(section.c_str(), L"Folder", path_.c_str());
    value.lastImage = readPath(section.c_str(), L"LastImage", path_.c_str());
    value.previousFolderImage = readPath(section.c_str(), L"PreviousFolderImage", path_.c_str());
    value.fit = GetPrivateProfileIntW(section.c_str(), L"FitHeight", 0, path_.c_str()) == 1
        ? PortraitFit::Height : PortraitFit::Width;
    value.participates = GetPrivateProfileIntW(section.c_str(), L"Participates", 1, path_.c_str()) != 0;
    value.uniqueImage = readPath(section.c_str(), L"UniqueImage", path_.c_str());
    value.uniqueActive = !value.uniqueImage.empty() &&
        GetPrivateProfileIntW(section.c_str(), L"UniqueActive", 0, path_.c_str()) == 1;
    return value;
}

RotationSettings Settings::loadRotation() const
{
    RotationSettings value;
    const UINT minutes = GetPrivateProfileIntW(L"Rotation", L"Minutes", value.minutes, path_.c_str());
    if (minutes >= 1 && minutes <= RotationSettings::maxMinutes) {
        value.minutes = minutes;
    }
    value.enabled = GetPrivateProfileIntW(L"Rotation", L"Enabled", 0, path_.c_str()) == 1;
    value.random = GetPrivateProfileIntW(L"Rotation", L"Random", 0, path_.c_str()) == 1;
    return value;
}

HRESULT Settings::saveMonitor(const Monitor& monitor, const MonitorSettings& value) const
{
    std::wstring fields;
    appendField(fields, L"Folder", value.folder.c_str());
    appendField(fields, L"LastImage", value.lastImage.c_str());
    appendField(fields, L"PreviousFolderImage", value.previousFolderImage.c_str());
    appendField(fields, L"FitHeight", value.fit == PortraitFit::Height ? L"1" : L"0");
    appendField(fields, L"Participates", value.participates ? L"1" : L"0");
    appendField(fields, L"UniqueImage", value.uniqueImage.c_str());
    appendField(fields, L"UniqueActive", value.uniqueActive ? L"1" : L"0");
    return save(monitorSection(monitor).c_str(), std::move(fields));
}

HRESULT Settings::saveRotation(const RotationSettings& value) const
{
    std::wstring fields;
    appendField(fields, L"Minutes", std::to_wstring(value.minutes).c_str());
    appendField(fields, L"Enabled", value.enabled ? L"1" : L"0");
    appendField(fields, L"Random", value.random ? L"1" : L"0");
    return save(L"Rotation", std::move(fields));
}

HRESULT Settings::save(PCWSTR section, std::wstring fields) const
{
    // The profile API preserves the existing encoding, so initialize new files as UTF-16.
    const HANDLE file = CreateFileW(path_.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                                    FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        const wchar_t bom = 0xfeff;
        DWORD written{};
        const BOOL ok = WriteFile(file, &bom, sizeof(bom), &written, nullptr);
        const DWORD error = ok ? ERROR_WRITE_FAULT : GetLastError();
        CloseHandle(file);
        if (written != sizeof(bom)) {
            DeleteFileW(path_.c_str());
            return HRESULT_FROM_WIN32(error);
        }
    } else if (const DWORD error = GetLastError(); error != ERROR_FILE_EXISTS) {
        return HRESULT_FROM_WIN32(error);
    }

    SetLastError(ERROR_SUCCESS);
    if (!WritePrivateProfileSectionW(section, fields.c_str(), path_.c_str())) {
        const DWORD error = GetLastError();
        return error ? HRESULT_FROM_WIN32(error) : E_FAIL;
    }
    return S_OK;
}
