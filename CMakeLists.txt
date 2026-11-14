cmake_minimum_required(VERSION 4.2)
project(PlopWallpaper LANGUAGES CXX)

if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_VS_PLATFORM_NAME STREQUAL "x64")
    message(FATAL_ERROR "Utilisez le générateur Visual Studio avec -A x64.")
endif()

add_executable(PlopWallpaper WIN32 src/main.cpp src/monitors.cpp)
target_compile_features(PlopWallpaper PRIVATE cxx_std_20)
target_compile_definitions(PlopWallpaper PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(PlopWallpaper PRIVATE /W4 /permissive- /utf-8)
target_link_libraries(PlopWallpaper PRIVATE ole32 uuid user32 gdi32)
