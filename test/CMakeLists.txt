cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0110 NEW) # allow dynamic test names

project(htest)

file(GLOB TEST_SOURCES ${CMAKE_SOURCE_DIR}/test/src/*.c)
file(GLOB TEST_INCLUDES ${CMAKE_SOURCE_DIR}/test/include/*.h)

foreach(TEST_SOURCE IN LISTS TEST_SOURCES)
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE} ${TEST_INCLUDES})
    target_link_libraries(${TEST_NAME} PRIVATE hedgehog_lib)
    target_include_directories(${TEST_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    if (NOT ${TEST_NAME} STREQUAL "test_prog")
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endif()
endforeach()

file(GLOB TEST_FILES ${CMAKE_CURRENT_SOURCE_DIR}/testcases/*.hhg)
foreach(TEST_PATH IN LISTS TEST_FILES)
  get_filename_component(TEST_NAME ${TEST_PATH} NAME)

  add_test(
    NAME ${TEST_NAME}
    COMMAND test_prog ${TEST_PATH}
  )
endforeach()