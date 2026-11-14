#pragma once

#include "wallpapers.h"

struct MonitorSettings {
    std::filesystem::path folder;
    std::filesystem::path lastImage;
    std::filesystem::path previousFolderImage;
    PortraitFit fit = PortraitFit::Width;
    bool rotationMode = true;
    std::filesystem::path uniqueImage;
    bool uniqueActive = false;

    bool rotates() const { return rotationMode && !folder.empty(); }

    void changeFolder(std::filesystem::path newFolder)
    {
        folder = std::move(newFolder);
        if (!lastImage.empty()) {
            previousFolderImage = std::move(lastImage);
            lastImage.clear();
        }
    }

    const std::filesystem::path& currentImage() const
    {
        if (uniqueActive) {
            return uniqueImage;
        }
        return lastImage.empty() ? previousFolderImage : lastImage;
    }
};

struct RotationSettings {
    static constexpr UINT maxMinutes = USER_TIMER_MAXIMUM / 60000;
    UINT minutes = 5;
    bool random = false;
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
