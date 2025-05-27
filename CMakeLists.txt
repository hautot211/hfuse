cmake_minimum_required(VERSION 3.10)

project(hfuse)
set(CMAKE_C_STANDARD 11)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

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
install(TARGETS hfuse)


add_executable(hello
  src/hello.c
)
target_include_directories(hello PRIVATE include)
target_link_directories(hello PRIVATE lib)
target_link_libraries(hello PRIVATE hfs fuse3)


add_subdirectory(test VARIABLES)
