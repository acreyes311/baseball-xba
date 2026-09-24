# Fetch Catch2's source at configure time, instead of vendoring it like rapidcsv.hpp.
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.16.0
)
FetchContent_MakeAvailable(Catch2)

# Point CMake at Catch2's CTest helper script.
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(xba_tests test_bucket_model.cpp test_batted_ball.cpp test_pitcher_xba.cpp test_pitcher_lookup.cpp test_stat_card.cpp)
target_link_libraries(xba_tests PRIVATE xba_core Catch2::Catch2WithMain)

# One CTest entry per TEST_CASE, instead of a single pass/fail for the whole binary.
# WORKING_DIRECTORY lets tests open fixture files with a plain relative path.
catch_discover_tests(xba_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
