cmake_minimum_required(VERSION 3.26)
project(files C)

set(CMAKE_C_STANDARD 11)

add_executable(files fileops.jsdrisco.c fileops.jsdrisco.h main.c)
add_executable(test fileops.jsdrisco.c fileops.jsdrisco.h fileops-test.c)