#include <catch2/catch_test_macros.hpp>

#include "pitcher_lookup.hpp"

namespace
{
    // The name search only reads pitcherName, and the tests use pitcherId to tell
    // matches apart, so the stat fields get default values.
    PitcherXba MakePitcher(long pitcherId, std::string pitcherName)
    {
        return PitcherXba{pitcherId, std::move(pitcherName), 0, 0.0, std::nullopt};
    }

    // Names use the same "Last, First" format as the real Statcast data.
    // Two pitchers share a last name so a search can return more than one match.
    std::vector<PitcherXba> MakeLeague()
    {
        return {
            MakePitcher(1, "Skenes, Paul"),
            MakePitcher(2, "Smith, Will"),
            MakePitcher(3, "Smith, Cade"),
        };
    }
} // namespace

// FIND PITCHERS BY NAME TESTS
TEST_CASE("FindPitchersByName matches a name at index 0", "[pitcher_lookup]")
{
    // "Skenes" starts at index 0 of "Skenes, Paul". This catches the bug of
    // writing "if (name.find(query))" instead of comparing against std::string::npos.
    const std::vector<PitcherXba> pitchers = MakeLeague();

    const std::vector<PitcherXba> matches = FindPitchersByName(pitchers, "Skenes");

    REQUIRE(matches.size() == 1);
    REQUIRE(matches[0].pitcherId == 1);
}

TEST_CASE("FindPitchersByName ignores upper/lower case", "[pitcher_lookup]")
{
    const std::vector<PitcherXba> pitchers = MakeLeague();

    // All lowercase, all uppercase, and a first name (middle of the string).
    REQUIRE(FindPitchersByName(pitchers, "skenes").size() == 1);
    REQUIRE(FindPitchersByName(pitchers, "SKENES").size() == 1);
    REQUIRE(FindPitchersByName(pitchers, "pAuL").size() == 1);
}

TEST_CASE("FindPitchersByName returns every pitcher whose name matches", "[pitcher_lookup]")
{
    const std::vector<PitcherXba> pitchers = MakeLeague();

    const std::vector<PitcherXba> matches = FindPitchersByName(pitchers, "smith");

    REQUIRE(matches.size() == 2);
    REQUIRE(matches[0].pitcherId == 2);
    REQUIRE(matches[1].pitcherId == 3);
}

TEST_CASE("FindPitchersByName returns nothing when no name matches", "[pitcher_lookup]")
{
    const std::vector<PitcherXba> pitchers = MakeLeague();

    REQUIRE(FindPitchersByName(pitchers, "Cole").empty());
}

TEST_CASE("FindPitchersByName returns nothing for an empty query", "[pitcher_lookup]")
{
    // "" is technically contained in every string, so without the empty-query
    // check this would return all three pitchers.
    const std::vector<PitcherXba> pitchers = MakeLeague();

    REQUIRE(FindPitchersByName(pitchers, "").empty());
}
