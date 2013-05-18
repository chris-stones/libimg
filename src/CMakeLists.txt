cmake_minimum_required(VERSION 2.8)

project(img)

INCLUDE(CPack)

add_definitions(-DWITH_P6_READ)
add_definitions(-DWITH_DDS_READ)
add_definitions(-DWITH_PNG_READ)
add_definitions(-DWITH_TGA_READ)
add_definitions(-DWITH_BMP_READ)

add_definitions(-DWITH_P6_WRITE)
add_definitions(-DWITH_DDS_WRITE)
add_definitions(-DWITH_PNG_WRITE)
add_definitions(-DWITH_TGA_WRITE)
add_definitions(-DWITH_BMP_WRITE)

include_directories ("${PROJECT_SOURCE_DIR}")

FILE(GLOB c_source_files *.c)

add_library(img SHARED ${c_source_files} )

install (TARGETS img DESTINATION lib)
install (FILES libimg.h DESTINATION include)

target_link_libraries (img png)
