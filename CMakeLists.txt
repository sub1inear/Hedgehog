cmake_minimum_required(VERSION 3.16)

project(hedgehog LANGUAGES C)
set(EXECUTABLE_NAME hhg)

set(CMAKE_C_STANDARD 99)

# enable testing
enable_testing()

# add all source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.c)

# add test subproject
add_subdirectory(test)

# create executable
add_executable(${EXECUTABLE_NAME} ${SOURCES})

# set library specific sources
set(LIB_SOURCES ${SOURCES})

# Remove main.c from the library sources
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

# create library
add_library(${EXECUTABLE_NAME}_lib STATIC ${LIB_SOURCES})

# add include directories
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME}_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hstdlib/lib)
target_include_directories(${EXECUTABLE_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hstdlib/lib)
target_include_directories(${EXECUTABLE_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib)
target_include_directories(${EXECUTABLE_NAME}_lib PUBLIC ${CMAKE_SOURCE_DIR}/lib)

# set hedgehog project to be the startup project in Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EXECUTABLE_NAME})

# enable warnings
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR
    CMAKE_C_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_C_COMPILER_ID STREQUAL "AppleClang" OR
    CMAKE_C_COMPILER_ID MATCHES "Intel" OR
    CMAKE_C_COMPILER_ID STREQUAL "IntelLLVM")
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /W4 /permissive- /WX /wd4996 /wd4063 /wd4206)
else()
    # Unknown compiler - intentionally do nothing (default to no warnings)
    message(STATUS "compiler '${CMAKE_C_COMPILER_ID}' not recognized   no warning flags added for target '${PROJECT_NAME}'")
endif()