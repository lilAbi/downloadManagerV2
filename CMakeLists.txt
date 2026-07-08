cmake_minimum_required(VERSION 4.3)
project(downloadManagerV2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_MESSAGE_LOG_LEVEL STATUS)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Important for GCC on macOS:
# We are not using C++20/C++23 modules in this GDExtension.
# Leaving CMake module scanning enabled causes GCC to compile with
# -fmodules-ts / P1689 scanning flags, which currently trips over
# Apple SDK headers such as _string.h.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_MODULE_STD OFF)

find_package(SDL3)
find_package(CURL)
find_package(spdlog)
find_package(imgui)

file(GLOB_RECURSE src_files
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.[hc]pp"
        "${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.[hc]"
        "${CMAKE_CURRENT_SOURCE_DIR}/bindings/*.[hc]pp"
)

add_executable(downloadManagerV2 ${src_files})

target_include_directories(downloadManagerV2
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/bindings
        src
)

target_link_libraries(downloadManagerV2
        PRIVATE
        sdl::sdl
        CURL::libcurl
        spdlog::spdlog
        imgui::imgui
)

set_target_properties(downloadManagerV2
        PROPERTIES
        # Generate position-independent code (PIC), necessary for shared/dynamic libraries.
        POSITION_INDEPENDENT_CODE ON
        # Enable interprocedural optimization for Release builds.
        INTERPROCEDURAL_OPTIMIZATION_RELEASE ON
)