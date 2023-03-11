cmake_minimum_required(VERSION 3.13)

project(naagtro.exe)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_library(lowti STATIC low.c ticker.c)
# Not required for build, only so QtCreator can find headers
target_include_directories(lowti PUBLIC /home/mikolaj/local/opt/djgpp-12.1.0/i586-pc-msdosdjgpp/sys-include)

add_executable(naagtro.exe main.c bumpmap.c data.c scroll.c tunnel.c)
target_link_libraries(naagtro.exe PUBLIC lowti)
