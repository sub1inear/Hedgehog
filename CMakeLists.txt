cmake_minimum_required(VERSION 3.16)

project(hedgehog)

set(CMAKE_CXX_STANDARD 20)

# add all source files
file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.cpp)

# add hstdlib subproject
add_subdirectory(hstdlib)

add_executable(${PROJECT_NAME} ${SOURCES})

# add include directories
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/hstdlib/lib)

# set working directory for VS next to executable (initially in build directory)
set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${PROJECT_NAME}>)