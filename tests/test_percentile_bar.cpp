#include <catch2/catch_test_macros.hpp>

#include <string>

#include "percentile_bar.hpp"

namespace
{
    // Builds the bar a test expects: `filled` full blocks, then `empty` empty ones.
    std::string ExpectedBar(int filled, int empty)
    {
        std::string bar;
        for (int i = 0; i < filled; ++i)
        {
            bar += "█";
        }
        for (int i = 0; i < empty; ++i)
        {
            bar += "░";
        }
        return bar;
    }
} // namespace

// Empty and full bar test
TEST_CASE("PercentileBar is empty at 0 and full at 100", "[percentile_bar]")
{
    REQUIRE(PercentileBar(0.0) == ExpectedBar(0, 20));
    REQUIRE(PercentileBar(100.0) == ExpectedBar(20, 0));
}

// Half bar test
TEST_CASE("PercentileBar fills half the bar at 50", "[percentile_bar]")
{
    REQUIRE(PercentileBar(50.0) == ExpectedBar(10, 10));
}

// Rounding test
TEST_CASE("PercentileBar rounds to the nearest block", "[percentile_bar]")
{
    // 92% of 20 is 18.4 blocks, which rounds down to 18.
    REQUIRE(PercentileBar(92.0) == ExpectedBar(18, 2));

    // 93% of 20 is 18.6 blocks, which rounds up to 19.
    REQUIRE(PercentileBar(93.0) == ExpectedBar(19, 1));
}

// Out of range test
TEST_CASE("PercentileBar treats values outside 0-100 as 0 or 100", "[percentile_bar]")
{
    REQUIRE(PercentileBar(-10.0) == ExpectedBar(0, 20));
    REQUIRE(PercentileBar(150.0) == ExpectedBar(20, 0));
}

// Custom width test
TEST_CASE("PercentileBar uses the width it is given", "[percentile_bar]")
{
    // 60% of 5 blocks is 3 blocks.
    REQUIRE(PercentileBar(60.0, 5) == ExpectedBar(3, 2));
}

// Byte size test
TEST_CASE("PercentileBar blocks take 3 bytes each", "[percentile_bar]")
{
    // A 20 block bar is 60 bytes, so size() can't be used to measure the width on screen.
    REQUIRE(PercentileBar(50.0).size() == 60);
}
