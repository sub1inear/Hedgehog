cmake_minimum_required(VERSION 3.16)
project(hstdlib)

# header-only interface library
add_library(hstdlib INTERFACE)

# expose the include folder to consumers
target_include_directories(hstdlib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})