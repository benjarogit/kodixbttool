# Findlibsquish.cmake - Find libsquish (e.g. from vcpkg on Windows; vcpkg exports unofficial-libsquish)
# Defines libsquish::libsquish imported target and libsquish_FOUND.

include(FindPackageHandleStandardArgs)

find_path(libsquish_INCLUDE_DIR NAMES squish.h
    PATHS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES include
    NO_DEFAULT_PATH)
if(NOT libsquish_INCLUDE_DIR)
    find_path(libsquish_INCLUDE_DIR NAMES squish.h)
endif()

find_library(libsquish_LIBRARY NAMES squish
    PATHS ${CMAKE_PREFIX_PATH}
    PATH_SUFFIXES lib
    NO_DEFAULT_PATH)
if(NOT libsquish_LIBRARY)
    find_library(libsquish_LIBRARY NAMES squish)
endif()

find_package_handle_standard_args(libsquish
    REQUIRED_VARS libsquish_LIBRARY libsquish_INCLUDE_DIR)

if(libsquish_FOUND AND NOT TARGET libsquish::libsquish)
    add_library(libsquish::libsquish UNKNOWN IMPORTED)
    set_target_properties(libsquish::libsquish PROPERTIES
        IMPORTED_LOCATION "${libsquish_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${libsquish_INCLUDE_DIR}")
endif()
