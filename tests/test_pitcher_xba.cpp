#include <algorithm>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "pitcher_xba.hpp"

namespace
{
    // ComputePitcherXba only reads pitcherId, pitcherName, launchSpeed, and launchAngle
    // (hit rate comes from looking the ball up in the buckets map, not from events),
    // so those are the only fields tests need to vary. Default values for the rest,
    // same pattern as MakeBall in test_bucket_model.cpp.
    BattedBall MakeBall(long pitcherId, std::string pitcherName, double launchSpeed, int launchAngle)
    {
        return BattedBall{"2024-01-01", std::move(pitcherName), pitcherId,
                          launchSpeed, launchAngle, "", std::nullopt};
    }

    // Builds a PitcherXba directly, skipping ComputePitcherXba.
    PitcherXba MakePitcher(long pitcherId, int battedBallCount, double xbaAllowed)
    {
        // percentile starts unset; that's AssignPercentiles's job to fill in.
        return PitcherXba{pitcherId, "Test Pitcher", battedBallCount, xbaAllowed, std::nullopt};
    }

    // Looks a pitcher up by id, since output order isn't guaranteed.
    const PitcherXba &FindPitcher(const std::vector<PitcherXba> &pitchers, long pitcherId)
    {
        // Assumes pitcherId is present; only used in tests that just added it.
        return *std::find_if(pitchers.begin(), pitchers.end(),
                             [pitcherId](const PitcherXba &p)
                             { return p.pitcherId == pitcherId; });
    }
}

// COMPUTE PITCHER XBA TESTS
TEST_CASE("ComputePitcherXba averages bucket hit rates across multiple balls for one pitcher", "[pitcher_xba]")
{
    // Two balls from the same pitcher, landing in two different buckets with
    // known hit rates: 1/2 = 0.5 and 3/4 = 0.75. xbaAllowed should be their average.
    std::map<BucketKey, BucketStats> buckets = {
        {BucketFor(90.0, 10), BucketStats{1, 2}},
        {BucketFor(50.0, 10), BucketStats{3, 4}},
    };
    std::vector<BattedBall> balls = {
        MakeBall(1, "Test Pitcher", 90.0, 10),
        MakeBall(1, "Test Pitcher", 50.0, 10),
    };

    std::vector<PitcherXba> pitchers = ComputePitcherXba(balls, buckets);

    REQUIRE(pitchers.size() == 1);
    REQUIRE(pitchers[0].pitcherId == 1);
    REQUIRE(pitchers[0].pitcherName == "Test Pitcher");
    REQUIRE(pitchers[0].battedBallCount == 2);
    REQUIRE(pitchers[0].xbaAllowed == Catch::Approx(0.625)); // (0.5 + 0.75) / 2
    REQUIRE_FALSE(pitchers[0].percentile.has_value());       // ComputePitcherXba never sets this
}

TEST_CASE("ComputePitcherXba keeps separate totals for multiple distinct pitchers", "[pitcher_xba]")
{
    // Two different pitchers, one ball each, both landing in the same bucket.
    // Proves grouping is per-pitcherId, not lumped together globally.
    std::map<BucketKey, BucketStats> buckets = {
        {BucketFor(90.0, 10), BucketStats{2, 5}}, // hit rate 0.4
    };
    std::vector<BattedBall> balls = {
        MakeBall(1, "Pitcher One", 90.0, 10),
        MakeBall(2, "Pitcher Two", 90.0, 10),
    };

    std::vector<PitcherXba> pitchers = ComputePitcherXba(balls, buckets);

    REQUIRE(pitchers.size() == 2);

    const PitcherXba &pitcherOne = FindPitcher(pitchers, 1);
    REQUIRE(pitcherOne.pitcherName == "Pitcher One");
    REQUIRE(pitcherOne.battedBallCount == 1);
    REQUIRE(pitcherOne.xbaAllowed == Catch::Approx(0.4));

    const PitcherXba &pitcherTwo = FindPitcher(pitchers, 2);
    REQUIRE(pitcherTwo.pitcherName == "Pitcher Two");
    REQUIRE(pitcherTwo.battedBallCount == 1);
    REQUIRE(pitcherTwo.xbaAllowed == Catch::Approx(0.4));
}

TEST_CASE("ComputePitcherXba returns an empty vector for no batted balls", "[pitcher_xba]")
{
    REQUIRE(ComputePitcherXba({}, {}).empty());
}

TEST_CASE("ComputePitcherXba throws std::out_of_range when a ball's bucket is missing from buckets", "[pitcher_xba]")
{
    // buckets is empty, so BucketFor(90.0, 10)'s key won't be found. buckets.at()
    // throws in that case, instead of silently treating the bucket as empty.
    std::vector<BattedBall> balls = {MakeBall(1, "Test Pitcher", 90.0, 10)};

    REQUIRE_THROWS_AS(ComputePitcherXba(balls, {}), std::out_of_range);
}

// PERCENTILE RANKING TESTS
TEST_CASE("PercentileRank counts strictly-less entries in a list with duplicates", "[pitcher_xba]")
{
    // 0.3 sits at index 3 (the two 0.2s and the 0.1 all count as less than,
    // but 0.3 doesn't count itself), so 100 * 3/5 = 60.0.
    REQUIRE(PercentileRank(0.3, {0.1, 0.2, 0.2, 0.3, 0.4}) == Catch::Approx(60.0));
}

// Empty list
TEST_CASE("PercentileRank returns 0.0 for an empty sortedValues vector", "[pitcher_xba]")
{
    REQUIRE(PercentileRank(5.0, {}) == 0.0);
}

// Value below every entry
TEST_CASE("PercentileRank returns 0.0 when value is below every entry", "[pitcher_xba]")
{
    REQUIRE(PercentileRank(0.05, {0.1, 0.2, 0.3}) == Catch::Approx(0.0));
}

// Value above every entry
TEST_CASE("PercentileRank returns 100.0 when value is above every entry", "[pitcher_xba]")
{
    REQUIRE(PercentileRank(0.5, {0.1, 0.2, 0.3}) == Catch::Approx(100.0));
}

TEST_CASE("PercentileRank excludes an exact match from its own count", "[pitcher_xba]")
{
    // 0.2 is itself a duplicate in the list. Only the 0.1 counts as strictly
    // less, so 100 * 1/4 = 25.0 -- neither 0.2 counts toward its own rank.
    REQUIRE(PercentileRank(0.2, {0.1, 0.2, 0.2, 0.3}) == Catch::Approx(25.0));
}
