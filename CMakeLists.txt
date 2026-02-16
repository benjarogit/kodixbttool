cmake_minimum_required(VERSION 3.10)
project(kodixbttool CXX)

set(CMAKE_CXX_STANDARD 11)

# Optional: for fseeko64 on glibc
add_compile_definitions(_FILE_OFFSET_BITS=64)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(_GNU_SOURCE)
endif()

set(SOURCES main.cpp xbtf.cpp xbtf_pack.cpp skin_unused.cpp)
if(WIN32)
    list(APPEND SOURCES getopt_win.c)
endif()
add_executable(kodixbttool ${SOURCES})

target_include_directories(kodixbttool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

if(WIN32)
    # vcpkg's lzo port does not provide FindLZO/LZOConfig; use find_library/find_path.
    # Explicitly search CMAKE_PREFIX_PATH (set by vcpkg toolchain) so vcpkg-installed LZO is found.
    find_package(GIF REQUIRED)
    find_package(libsquish CONFIG REQUIRED)
    find_library(LZO2_LIB NAMES lzo2 lzo
        PATHS ${CMAKE_PREFIX_PATH}
        PATH_SUFFIXES lib
        NO_DEFAULT_PATH)
    find_path(LZO2_INCLUDE NAMES lzo/lzo1x.h
        PATHS ${CMAKE_PREFIX_PATH}
        PATH_SUFFIXES include
        NO_DEFAULT_PATH)
    if(NOT LZO2_LIB OR NOT LZO2_INCLUDE)
        message(FATAL_ERROR "LZO not found. Install with: vcpkg install lzo")
    endif()
    target_include_directories(kodixbttool PRIVATE ${LZO2_INCLUDE})
    target_link_libraries(kodixbttool PRIVATE
        PNG::PNG
        JPEG::JPEG
        GIF::GIF
        libsquish::libsquish
        ${LZO2_LIB}
    )
elseif(APPLE)
    # macOS/Homebrew: use FindGIF; LZO and squish often have no CMake config, use find_library
    find_package(GIF REQUIRED)
    find_library(LZO2_LIB NAMES lzo2)
    find_library(SQUISH_LIB NAMES squish)
    find_path(LZO2_INCLUDE NAMES lzo/lzo1x.h)
    find_path(SQUISH_INCLUDE NAMES squish.h)
    if(NOT LZO2_LIB OR NOT LZO2_INCLUDE)
        message(FATAL_ERROR "LZO not found. Install with: brew install lzo")
    endif()
    if(NOT SQUISH_LIB OR NOT SQUISH_INCLUDE)
        message(FATAL_ERROR "libsquish not found. Install with: brew install libsquish")
    endif()
    target_include_directories(kodixbttool PRIVATE ${LZO2_INCLUDE} ${SQUISH_INCLUDE})
    target_link_libraries(kodixbttool PRIVATE
        PNG::PNG
        JPEG::JPEG
        GIF::GIF
        ${LZO2_LIB}
        ${SQUISH_LIB}
        pthread
    )
else()
    # Linux: distro packages usually provide libs that CMake finds by name
    target_link_libraries(kodixbttool PRIVATE
        PNG::PNG
        JPEG::JPEG
        lzo2
        gif
        squish
        pthread
    )
endif()

install(TARGETS kodixbttool RUNTIME DESTINATION bin)
