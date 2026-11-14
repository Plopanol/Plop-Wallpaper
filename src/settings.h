#pragma once

#include "wallpapers.h"

struct MonitorSettings {
    std::filesystem::path folder;
    std::filesystem::path lastImage;
    PortraitFit fit = PortraitFit::Width;
    bool participates = true;
};

struct RotationSettings {
    static constexpr UINT maxMinutes = USER_TIMER_MAXIMUM / 60000;
    UINT minutes = 5;
    bool enabled = false;
};

class Settings {
public:
    explicit Settings(std::filesystem::path path);
    MonitorSettings loadMonitor(const Monitor& monitor) const;
    RotationSettings loadRotation() const;
    HRESULT saveMonitor(const Monitor& monitor, const MonitorSettings& value) const;
    HRESULT saveRotation(const RotationSettings& value) const;

private:
    HRESULT save(PCWSTR section, std::wstring fields) const;
    std::filesystem::path path_;
};
