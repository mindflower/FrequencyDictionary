cmake_minimum_required(VERSION 3.8)

project(dictionary)
find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(BUILD_GMOCK OFF CACHE BOOL "Build only googletest")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(freq)

add_dependencies(tests freq_lib)
add_dependencies(freq freq_lib)