#pragma once

#include <windows.h>
#include <filesystem>

HRESULT readStartupEnabled(bool& enabled);
HRESULT setStartupEnabled(const std::filesystem::path& executable, bool enabled);
