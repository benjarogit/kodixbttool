cmake_minimum_required(VERSION 3.10)
project(kodixbttool CXX)

set(CMAKE_CXX_STANDARD 11)

# Optional: for fseeko64 on glibc
add_compile_definitions(_FILE_OFFSET_BITS=64)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_compile_definitions(_GNU_SOURCE)
endif()

add_executable(kodixbttool main.cpp xbtf.cpp xbtf_pack.cpp skin_unused.cpp)

target_include_directories(kodixbttool PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)

target_link_libraries(kodixbttool PRIVATE
    PNG::PNG
    JPEG::JPEG
    lzo2
    gif
    squish
    pthread
)

install(TARGETS kodixbttool RUNTIME DESTINATION bin)
