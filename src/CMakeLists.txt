cmake_minimum_required(VERSION 2.8)

project(img)

INCLUDE(CPack)

include_directories ("${PROJECT_SOURCE_DIR}")

FILE(GLOB c_source_files *.c)

add_library(img SHARED ${c_source_files} )

install (TARGETS img DESTINATION lib)
install (FILES libimg.h DESTINATION include)

target_link_libraries (img png)
