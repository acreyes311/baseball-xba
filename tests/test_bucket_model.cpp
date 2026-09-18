#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp> // TEST_CASE, REQUIRE, etc.

#include "bucket_model.hpp"

namespace
{
    // BattedBall has 7 fields(batted_ball.hpp), but only 3 are relevant to the bucket model
    // (launchSpeed, launchAngle, events). Used default values for other fields to simplify test cases.
    BattedBall MakeBall(double launchSpeed, int launchAngle, std::string events)
    {
        return BattedBall{"2024-01-01", "Test Player", 1,
                          launchSpeed, launchAngle, std::move(events), std::nullopt};
    }
} // namespace

TEST_CASE("BucketFor floors velocity to the nearest 5 mph bucket", "[bucket_model]")
{
    // Angle fixed at 0 so only the velocity bucket (.first) is under test.
    REQUIRE(BucketFor(90.0, 0).first == 18); // exact multiple starts a new bucket
    REQUIRE(BucketFor(89.9, 0).first == 17); // just below that boundary
    REQUIRE(BucketFor(94.9, 0).first == 18); // just below the *next* boundary
    REQUIRE(BucketFor(0.0, 0).first == 0);
}

TEST_CASE("BucketFor floors angle using floor division so negatives bucket correctly", "[bucket_model]")
{
    // Velocity fixed at 90.0 (bucket 18) so only the angle bucket (.second) is under test.
    REQUIRE(BucketFor(90.0, 0).second == 0);
    REQUIRE(BucketFor(90.0, 4).second == 0);   // just below the boundary
    REQUIRE(BucketFor(90.0, 5).second == 1);   // exact boundary starts a new bucket
    REQUIRE(BucketFor(90.0, -1).second == -1); // truncating division would wrongly give 0
    REQUIRE(BucketFor(90.0, -5).second == -1); // exact negative boundary
    REQUIRE(BucketFor(90.0, -6).second == -2);
}

TEST_CASE("HitRate returns 0 when a bucket has no batted balls", "[bucket_model]")
{
    // BucketStats has default values of 0 for hits and total, so this is a bucket with no batted balls.
    REQUIRE(BucketStats{}.HitRate() == 0.0);
}

TEST_CASE("HitRate divides hits by total for a populated bucket", "[bucket_model]")
{
    // BucketStats with 1 hit and 3 total batted balls should return 1/3.
    REQUIRE(BucketStats{1, 3}.HitRate() == Catch::Approx(1.0 / 3.0));
}

TEST_CASE("BuildBucketModel returns an empty map for no batted balls", "[bucket_model]")
{
    // An empty vector of batted balls should produce an empty bucket model.
    REQUIRE(BuildBucketModel({}).empty());
}

TEST_CASE("BuildBucketModel groups balls by bucket and counts totals", "[bucket_model]")
{
    // 3 batted balls, 2 in the same bucket and 1 in a different bucket
    std::vector<BattedBall> balls = {
        MakeBall(90.0, 10, "field_out"), // bucket (18, 2)
        MakeBall(91.0, 11, "single"),    // same bucket (18, 2)
        MakeBall(50.0, 10, "field_out"), // different bucket (10, 2)
    };

    std::map<BucketKey, BucketStats> buckets = BuildBucketModel(balls);

    REQUIRE(buckets.size() == 2);
    REQUIRE(buckets[{18, 2}].total == 2);
    REQUIRE(buckets[{10, 2}].total == 1);
}

TEST_CASE("BuildBucketModel counts only exact-match hit events as hits", "[bucket_model]")
{
    // 6 batted balls, 4 of which are hits and 2 of which are not hits
    std::vector<BattedBall> balls = {
        MakeBall(90.0, 0, "single"),
        MakeBall(90.0, 0, "double"),
        MakeBall(90.0, 0, "triple"),
        MakeBall(90.0, 0, "home_run"),
        MakeBall(90.0, 0, "field_out"),
        MakeBall(90.0, 0, ""),
    };

    BucketStats stats = BuildBucketModel(balls).at({18, 0});

    REQUIRE(stats.total == 6);
    REQUIRE(stats.hits == 4);
    REQUIRE(stats.HitRate() == Catch::Approx(4.0 / 6.0));
}
