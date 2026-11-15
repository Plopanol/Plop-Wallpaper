#include "settings.h"
#include "tray_click.h"

#include <iostream>
#include <string_view>

namespace {

class DesktopWallpaper final : public IDesktopWallpaper {
public:
    UINT count = 2;
    HRESULT countResult = S_OK;

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID, void**) override { return E_NOINTERFACE; }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE SetWallpaper(LPCWSTR, LPCWSTR) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetWallpaper(LPCWSTR, LPWSTR*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetMonitorDevicePathCount(UINT* result) override
    {
        *result = count;
        return countResult;
    }
    HRESULT STDMETHODCALLTYPE GetMonitorDevicePathAt(UINT index, LPWSTR* result) override
    {
        *result = static_cast<LPWSTR>(CoTaskMemAlloc(2 * sizeof(wchar_t)));
        if (!*result) {
            return E_OUTOFMEMORY;
        }
        (*result)[0] = index == 0 ? L'A' : L'B';
        (*result)[1] = L'\0';
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE GetMonitorRECT(LPCWSTR, RECT* rect) override
    {
        *rect = {0, 0, 1920, 1080};
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE SetBackgroundColor(COLORREF) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetBackgroundColor(COLORREF*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetPosition(DESKTOP_WALLPAPER_POSITION) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetPosition(DESKTOP_WALLPAPER_POSITION*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetSlideshow(IShellItemArray*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetSlideshow(IShellItemArray**) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE SetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS, UINT) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetSlideshowOptions(DESKTOP_SLIDESHOW_OPTIONS*, UINT*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE AdvanceSlideshow(LPCWSTR, DESKTOP_SLIDESHOW_DIRECTION) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE GetStatus(DESKTOP_SLIDESHOW_STATE*) override { return E_NOTIMPL; }
    HRESULT STDMETHODCALLTYPE Enable(BOOL) override { return E_NOTIMPL; }
};

bool monitorTopology()
{
    DesktopWallpaper wallpaper;
    std::vector<Monitor> monitors;
    if (readMonitors(wallpaper, monitors) != S_OK || monitors.size() != 2) {
        return false;
    }
    wallpaper.count = 1;
    if (readMonitors(wallpaper, monitors) != S_OK || monitors.size() != 1 || monitors[0].id != L"A") {
        return false;
    }
    wallpaper.count = 2;
    return readMonitors(wallpaper, monitors) == S_OK && monitors.size() == 2 && monitors[1].id == L"B";
}

bool monitorDetectionRecovery()
{
    DesktopWallpaper wallpaper;
    std::vector<Monitor> monitors;
    wallpaper.countResult = E_FAIL;
    if (readMonitors(wallpaper, monitors) != E_FAIL || !monitors.empty()) {
        return false;
    }

    wallpaper.countResult = S_OK;
    return readMonitors(wallpaper, monitors) == S_OK && monitors.size() == 2 &&
        monitors[0].id == L"A" && monitors[1].id == L"B";
}

bool folderChange()
{
    MonitorSettings selection;
    selection.folder = L"A";
    selection.lastImage = L"A/image.png";
    const auto displayed = selection.currentImage();
    selection.changeFolder(L"B");
    if (selection.folder != L"B" || !selection.lastImage.empty() ||
        selection.currentImage() != displayed) {
        return false;
    }
    selection.changeFolder(L"C");
    if (selection.currentImage() != displayed) {
        return false;
    }
    selection.lastImage = L"C/new.png";
    if (selection.currentImage() != selection.lastImage) {
        return false;
    }
    selection.uniqueImage = L"unique.png";
    selection.uniqueActive = true;
    selection.changeFolder(L"D");
    return selection.currentImage() == selection.uniqueImage;
}

}

bool trayClick()
{
    TrayClick click;
    click.defer();
    click.cancel();
    if (click.take()) {
        return false;
    }
    click.defer();
    return click.take() && !click.take();
}

bool monitorMode()
{
    wchar_t directory[MAX_PATH]{};
    wchar_t path[MAX_PATH]{};
    const DWORD length = GetTempPathW(ARRAYSIZE(directory), directory);
    if (!length || length >= ARRAYSIZE(directory) || !GetTempFileNameW(directory, L"plp", 0, path)) {
        return false;
    }
    const Monitor first{L"A", L"A"};
    const Monitor second{L"B", L"B"};
    const Settings settings{path};
    const bool prepared =
        WritePrivateProfileStringW(L"Rotation", L"Enabled", L"1", path) &&
        WritePrivateProfileStringW(L"Monitor:0041", L"Folder", L"A", path) &&
        WritePrivateProfileStringW(L"Monitor:0042", L"Folder", L"B", path) &&
        WritePrivateProfileStringW(L"Monitor:0042", L"Participates", L"0", path) &&
        WritePrivateProfileStringW(L"Monitor:0042", L"LastImage", L"B/image.png", path);
    auto fixed = settings.loadMonitor(second);
    const bool migrated = settings.loadMonitor(first).rotates() && !fixed.rotationMode &&
        fixed.currentImage() == L"B/image.png";
    fixed.rotationMode = true;
    const bool persisted = settings.saveRotation(settings.loadRotation()) == S_OK &&
        settings.saveMonitor(second, fixed) == S_OK &&
        settings.loadMonitor(first).rotates() && settings.loadMonitor(second).rotates();
    DeleteFileW(path);
    return prepared && migrated && persisted;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        return 2;
    }
    const std::string_view test = argv[1];
    if (test == "BUG-006") {
        return monitorDetectionRecovery() ? 0 : 1;
    }
    if (test == "TASK-006-tray-click") {
        return trayClick() ? 0 : 1;
    }
    if (test == "TASK-006-monitor-mode") {
        return monitorMode() ? 0 : 1;
    }
    if (test != "BUG-002" && test != "BUG-003") {
        return 2;
    }
    const bool passed = test == "BUG-002" ? monitorTopology() : folderChange();
    if (!passed) {
        std::cerr << test << '\n';
    }
    return passed ? 0 : 1;
}
