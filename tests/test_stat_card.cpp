#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <utility>
#include <vector>

#include "stat_card.hpp"

using Catch::Matchers::ContainsSubstring;

// Full card test
TEST_CASE("FormatStatCard prints every field for a qualified pitcher", "[stat_card]")
{
    const PitcherXba pitcher{694973, "Skenes, Paul", 84, 0.215, 92.0};

    // The whole card is checked here, so any change to its layout shows up as a failure.
    REQUIRE(FormatStatCard(pitcher) == "Skenes, Paul (id 694973)\n"
                                       "  xBA allowed:   .215\n"
                                       "  Batted balls:  84\n"
                                       "  Percentile:    92nd\n");
}

// xBA format test
TEST_CASE("FormatStatCard rounds xBA to 3 digits with no leading zero", "[stat_card]")
{
    const PitcherXba pitcher{1, "Test, Pitcher", 30, 0.2146, 50.0};
    // Check that this string is present somewhere in the output
    REQUIRE_THAT(FormatStatCard(pitcher), ContainsSubstring("xBA allowed:   .215\n"));
}

// The ordinal test
TEST_CASE("FormatStatCard uses the right ordinal suffix", "[stat_card]")
{
    // 11, 12 and 13 are printed as 11th, not 11st.
    const std::vector<std::pair<double, std::string>> cases = {
        {1.0, "1st"},
        {2.0, "2nd"},
        {3.0, "3rd"},
        {4.0, "4th"},
        {11.0, "11th"},
        {12.0, "12th"},
        {13.0, "13th"},
        {21.0, "21st"},
        {22.0, "22nd"},
        {100.0, "100th"},
    };

    for (const auto &[percentile, expected] : cases)
    {
        // CAPTURE prints these values if a check fails, so we know which case broke.
        CAPTURE(percentile, expected);
        const PitcherXba pitcher{1, "Test, Pitcher", 30, 0.250, percentile};
        REQUIRE_THAT(FormatStatCard(pitcher), ContainsSubstring("Percentile:    " + expected + "\n"));
    }
}

// Percentile rounding test
TEST_CASE("FormatStatCard rounds the percentile to a whole number", "[stat_card]")
{
    const PitcherXba pitcher{1, "Test, Pitcher", 30, 0.250, 91.6};

    REQUIRE_THAT(FormatStatCard(pitcher), ContainsSubstring("Percentile:    92nd\n"));
}

// Missing percentile test
TEST_CASE("FormatStatCard shows n/a when the pitcher has no percentile", "[stat_card]")
{
    const PitcherXba pitcher{1, "Test, Pitcher", 12, 0.310, std::nullopt};

    REQUIRE_THAT(FormatStatCard(pitcher),
                 ContainsSubstring("Percentile:    n/a (needs 20+ batted balls)\n"));
}
