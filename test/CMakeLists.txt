# File        : CMakeLists.txt
#               ( CMake build file )
# 
# Description : GREYC's C project to mount standard HFS disk images on Linux virtual filesystem
# 
# Author   : Guillaume HAUTOT
# 
# License     : MIT
# 
# Copyright (c) 2025
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

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
create_test(trim_vdir)
create_test(escape_chars)
