cmake_minimum_required(VERSION 3.8)

include_directories(${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/include)
list(APPEND EXTRA_LIBS freq_lib gtest gtest_main)

add_executable(freq_tests dictonary_tests.cpp)
target_link_libraries(freq_tests ${EXTRA_LIBS})