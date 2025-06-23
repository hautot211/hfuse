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

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

project(hfuse)

include(CTest)
enable_testing()

# Source directories
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)

# Output directories
set(BIN_DIR ${PROJECT_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${BIN_DIR})
set(DIST_DIR ${PROJECT_BINARY_DIR}/dist)
file(MAKE_DIRECTORY ${DIST_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIST_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIST_DIR})

include_directories(${INCLUDE_DIR})

add_library(hfuse_lib
  src/hfs_helpers.c
  src/file_handler.c
  src/hfuse_context.c
  src/hfuse.c
  src/debug_funcs.c
)
target_include_directories(hfuse_lib PRIVATE include)
target_link_directories(hfuse_lib PRIVATE lib)
target_link_libraries(hfuse_lib PRIVATE hfs fuse3)


add_executable(hfuse
  src/main.c
)
target_include_directories(hfuse PRIVATE include)
target_link_directories(hfuse PRIVATE lib)
target_link_libraries(hfuse PRIVATE hfs fuse3)
target_link_libraries(hfuse PRIVATE hfs hfuse_lib)
install(TARGETS hfuse COMPONENT runtime)


add_subdirectory(${TEST_DIR})

