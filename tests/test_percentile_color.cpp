#include <catch2/catch_test_macros.hpp>

#include "percentile_color.hpp"

namespace
{
    // Checks all three channels of a percentile's color against the expected values.
    // CHECK keeps going after a failure, so every wrong channel shows up, not just the first.
    void CheckColor(double percentile, int red, int green, int blue)
    {
        CAPTURE(percentile);
        const Rgb color = PercentileColor(percentile);
        CHECK(color.red == red);
        CHECK(color.green == green);
        CHECK(color.blue == blue);
    }
} // namespace

// Fixed colors test
TEST_CASE("PercentileColor is blue at 0, grey at 50 and red at 100", "[percentile_color]")
{
    CheckColor(0.0, 50, 100, 200);
    CheckColor(50.0, 180, 180, 180);
    CheckColor(100.0, 215, 40, 40);
}

// Lower half test
TEST_CASE("PercentileColor blends blue into grey below 50", "[percentile_color]")
{
    // 25 is halfway from 0 to 50, so each channel is halfway from blue to grey.
    CheckColor(25.0, 115, 140, 190);
}

// Upper half test
TEST_CASE("PercentileColor blends grey into red above 50", "[percentile_color]")
{
    // 90 is 80% of the way from 50 to 100, so each channel moves 80% of the way from grey to red.
    CheckColor(90.0, 208, 68, 68);
}

// Rounding test
TEST_CASE("PercentileColor rounds each channel to the nearest whole number", "[percentile_color]")
{
    // 33 is 66% of the way from blue to grey: 135.8, 152.8 and 186.8, which all round up.
    CheckColor(33.0, 136, 153, 187);
}

// Out of range test
TEST_CASE("PercentileColor treats values outside 0-100 as 0 or 100", "[percentile_color]")
{
    CheckColor(-10.0, 50, 100, 200);
    CheckColor(150.0, 215, 40, 40);
}
