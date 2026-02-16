# FindLZO.cmake - Find LZO (e.g. from vcpkg on Windows; no LZOConfig provided by vcpkg)
# Defines LZO::LZO imported target and LZO_FOUND.

include(FindPackageHandleStandardArgs)

find_path(LZO_INCLUDE_DIR NAMES lzo/lzo1x.h
    PATHS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)
if(NOT LZO_INCLUDE_DIR)
    find_path(LZO_INCLUDE_DIR NAMES lzo/lzo1x.h)
endif()

find_library(LZO_LIBRARY NAMES lzo2 lzo
    PATHS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH)
if(NOT LZO_LIBRARY)
    find_library(LZO_LIBRARY NAMES lzo2 lzo)
endif()

find_package_handle_standard_args(LZO
    REQUIRED_VARS LZO_LIBRARY LZO_INCLUDE_DIR)

if(LZO_FOUND AND NOT TARGET LZO::LZO)
    add_library(LZO::LZO UNKNOWN IMPORTED)
    set_target_properties(LZO::LZO PROPERTIES
        IMPORTED_LOCATION "${LZO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LZO_INCLUDE_DIR}")
endif()
