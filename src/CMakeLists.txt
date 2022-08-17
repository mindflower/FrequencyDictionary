cmake_minimum_required(VERSION 3.8)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(freq_lib utils.cpp stringblockmapper.cpp fileblockmapper.cpp container.cpp controller.cpp worker.cpp dictionary.cpp)