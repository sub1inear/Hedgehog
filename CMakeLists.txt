cmake_minimum_required(VERSION 3.16)

project(hedgehog LANGUAGES C)

set(CMAKE_C_STANDARD 99)

# enable testing
enable_testing()

# add all source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.c)

# add hstdlib subproject
add_subdirectory(hstdlib)

# add test subproject
add_subdirectory(test)

# create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# add stb library
add_library(stb INTERFACE)

# set library specific sources
set(LIB_SOURCES ${SOURCES})

# Remove main.c from the library sources
list(REMOVE_ITEM LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c)

# create library
add_library(${PROJECT_NAME}_lib STATIC ${LIB_SOURCES})

# add include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hstdlib/lib)
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hstdlib/lib)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/stb)
target_include_directories(${PROJECT_NAME}_lib PUBLIC ${CMAKE_SOURCE_DIR}/lib/stb)


# set working directory for VS next to executable (initially in build directory)
# set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)