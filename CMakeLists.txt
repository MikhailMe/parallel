cmake_minimum_required(VERSION 3.8)

project(parallel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g2 -O0")

set(MATRIX_H matrix/Matrix.h helper.h)

add_executable(parallel main.cpp matrix/Matrix.cpp ${MATRIX_H})