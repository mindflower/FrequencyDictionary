cmake_minimum_required(VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(freq main.cpp)
target_link_libraries(freq freq_lib)