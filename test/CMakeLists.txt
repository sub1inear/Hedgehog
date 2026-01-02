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

# --- programmatically add tests for .hhg testcases ---
# It expects structure: testcases/<category>/<valid|invalid>/.../<file>.hhg
# Example: testcases/lexer/invalid/class/class.hhg -> name: "(lexer invalid) class.hhg"
file(GLOB_RECURSE HHG_TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/testcases/*.hhg)

foreach(HHG_REL_PATH IN LISTS HHG_TEST_FILES)
    # HHG_REL_PATH is relative to CMAKE_CURRENT_SOURCE_DIR, e.g. "testcases/lexer/invalid/class/class.hhg"
    # remove "testcases/" prefix to easily split components
    string(REGEX REPLACE "^testcases/?" "" REL_AFTER_ROOT ${HHG_REL_PATH})

    # split by '/'
    string(REPLACE "/" ";" PARTS ${REL_AFTER_ROOT})
    list(LENGTH PARTS PARTS_LEN)
    if (PARTS_LEN LESS 2)
        message(WARNING "Skipping test with unexpected layout: ${HHG_REL_PATH}")
        continue()
    endif()

    list(GET PARTS 0 CATEGORY)       # e.g. lexer
    list(GET PARTS 1 EXPECTATION)    # e.g. valid or invalid

    # sanitize expectation/category just in case
    string(STRIP ${CATEGORY} CATEGORY)
    string(STRIP ${EXPECTATION} EXPECTATION)

    # Decide whether this is an expected-fail test
    set(FAIL_FLAG "")
    if (EXPECTATION STREQUAL "invalid")
        set(FAIL_FLAG "-f")
    endif()

    # the display filename (last path component)
    get_filename_component(FILE_NAME ${HHG_REL_PATH} NAME) # e.g. class.hhg

    # test display name like "(lexer invalid) class.hhg"
    set(TEST_DISPLAY_NAME "(${CATEGORY} ${EXPECTATION}) ${FILE_NAME}")

    # build command arguments: optional -f, --<category>, full path to hhg file
    set(TEST_ARGS)
    if (FAIL_FLAG)
        list(APPEND TEST_ARGS ${FAIL_FLAG})
    endif()
    list(APPEND TEST_ARGS --${CATEGORY} ${CMAKE_CURRENT_SOURCE_DIR}/${HHG_REL_PATH})

    # register the test. Use the explicit command + args form.
    add_test(NAME "${TEST_DISPLAY_NAME}" COMMAND test_prog ${TEST_ARGS})
endforeach()
