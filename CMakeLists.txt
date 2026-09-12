cmake_minimum_required(VERSION 3.16)
project(baseball_xba CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(baseball_xba src/main.cpp src/batted_ball.cpp src/bucket_model.cpp)
target_include_directories(baseball_xba PRIVATE include)
