cmake_minimum_required(VERSION 3.13)

project(naagtro.exe)

set(GCC_COVERAGE_COMPILE_FLAGS "-W -Wall -pedantic")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

add_library(low STATIC low.c)
target_include_directories(low PUBLIC /home/mikolaj/local/opt/djgpp-12.1.0/i586-pc-msdosdjgpp/sys-include)

add_executable(naagtro.exe main.cpp data.cpp)
#target_include_directories(naagtro.exe PUBLIC)

target_link_libraries(naagtro.exe PUBLIC low)
