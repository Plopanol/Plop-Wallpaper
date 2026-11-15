#pragma once

#include <filesystem>
#include <string_view>

bool appendErrorLog(const std::filesystem::path& directory, std::wstring_view message);
void logError(const wchar_t* message) noexcept;
