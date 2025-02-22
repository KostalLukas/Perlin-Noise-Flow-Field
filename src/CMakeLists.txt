cmake_minimum_required(VERSION 3.22)
project(Perlin)

find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})

set(CMAKE_CXX_STANDARD 14)

add_executable(Perlin Perlin.cpp)

target_link_libraries(Perlin ${OpenCV_LIBS})
