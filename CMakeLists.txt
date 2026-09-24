cmake_minimum_required(VERSION 3.16)
project(baseball_xba CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(xba_core src/batted_ball.cpp src/bucket_model.cpp src/pitcher_xba.cpp src/pitcher_lookup.cpp)
target_include_directories(xba_core PUBLIC include)

add_executable(baseball_xba src/main.cpp)
target_link_libraries(baseball_xba PRIVATE xba_core)

enable_testing()
add_subdirectory(tests)
