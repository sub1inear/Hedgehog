cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0110 NEW) # allow dynamic test names

project(htest)

add_executable(${PROJECT_NAME} test.c)

target_link_libraries(${PROJECT_NAME} PRIVATE hedgehog_lib)

file(GLOB TEST_FILES ${CMAKE_SOURCE_DIR}/test/testcases/*)
foreach(TEST_PATH IN LISTS TEST_FILES)
  get_filename_component(TEST_NAME "${TEST_PATH}" NAME)

  add_test(
    NAME "${TEST_NAME}"
    COMMAND htest "${TEST_PATH}"
  )
endforeach()


# add minunit library
add_library(minunit INTERFACE)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/minunit)