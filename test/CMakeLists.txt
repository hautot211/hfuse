cmake_minimum_required(VERSION 3.10)

project(hfuse_tests)
set(CMAKE_C_STANDARD 11)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})


# Source directories
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

# Output directories
set(BIN_DIR ${PROJECT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

function(create_test test_name)
    set(TEST_OUTPUT_NAME "test_${test_name}")
    set(TEST_SOURCE_NAME "${SRC_DIR}/${test_name}.c")
    add_executable(${TEST_OUTPUT_NAME} ${TEST_SOURCE_NAME})
    target_include_directories(${TEST_OUTPUT_NAME} PRIVATE ${INCLUDE_DIR})
    target_link_directories(${TEST_OUTPUT_NAME} PRIVATE ${DIST_DIR})
    target_link_directories(${TEST_OUTPUT_NAME} PRIVATE lib)
    target_link_libraries(${TEST_OUTPUT_NAME} PRIVATE hfuse_lib)
    add_test(
        NAME ${TEST_OUTPUT_NAME}
        COMMAND ${TEST_OUTPUT_NAME}
        WORKING_DIRECTORY ${BIN_DIR}
    )
endfunction()

create_test(hfs_helpers)
create_test(test2)
create_test(test3)
