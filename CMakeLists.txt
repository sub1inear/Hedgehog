cmake_minimum_required(VERSION 3.16)

project(hedgehog)

set(CMAKE_CXX_STANDARD 20)

# add all source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.cpp)

add_subdirectory(hstdlib)

add_executable(${PROJECT_NAME} ${SOURCES})