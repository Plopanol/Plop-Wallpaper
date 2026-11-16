cmake_minimum_required(VERSION 4.2)
project(PlopWallpaper LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_VS_PLATFORM_NAME STREQUAL "x64")
    message(FATAL_ERROR "Utilisez le générateur Visual Studio avec -A x64.")
endif()

add_executable(PlopWallpaper WIN32
    src/app.rc src/error_log.cpp src/main.cpp src/monitors.cpp src/settings.cpp src/startup.cpp src/theme.cpp src/wallpapers.cpp)
set_property(TARGET PlopWallpaper PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
target_compile_features(PlopWallpaper PRIVATE cxx_std_20)
target_compile_definitions(PlopWallpaper PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(PlopWallpaper PRIVATE /W4 /permissive- /utf-8)
target_link_libraries(PlopWallpaper PRIVATE ole32 uuid user32 gdi32 shell32 windowscodecs dwmapi uxtheme windowsapp comctl32)
target_link_options(PlopWallpaper PRIVATE
    "/MANIFESTDEPENDENCY:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'")

include(CTest)
if(BUILD_TESTING)
    add_executable(PlopWallpaperTests tests/regressions.cpp src/error_log.cpp src/monitors.cpp src/settings.cpp)
    target_include_directories(PlopWallpaperTests PRIVATE src)
    target_compile_features(PlopWallpaperTests PRIVATE cxx_std_20)
    target_compile_definitions(PlopWallpaperTests PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    target_compile_options(PlopWallpaperTests PRIVATE /W4 /permissive- /utf-8)
    target_link_libraries(PlopWallpaperTests PRIVATE ole32 user32)
    add_test(NAME BUG-002 COMMAND PlopWallpaperTests BUG-002)
    add_test(NAME BUG-003 COMMAND PlopWallpaperTests BUG-003)
    add_test(NAME BUG-006 COMMAND PlopWallpaperTests BUG-006)
    add_test(NAME BUG-008 COMMAND pwsh -NoProfile -File
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/single_instance.ps1" -Executable "$<TARGET_FILE:PlopWallpaper>")
    add_test(NAME TASK-006-tray-click COMMAND PlopWallpaperTests TASK-006-tray-click)
    add_test(NAME TASK-006-monitor-mode COMMAND PlopWallpaperTests TASK-006-monitor-mode)
    add_test(NAME TASK-009 COMMAND PlopWallpaperTests TASK-009)
endif()
