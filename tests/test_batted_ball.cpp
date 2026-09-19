#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <stdexcept>

#include "batted_ball.hpp"

namespace
{
    const std::string kFixturePath = "fixtures/sample_batted_balls.csv";
}

TEST_CASE("LoadBattedBalls parses every row from the CSV, including negative angles", "[batted_ball]")
{
    // The fixture has 3 rows, including one with a negative launch angle.
    // This test confirms that all rows are parsed and that the negative angle is handled correctly.
    std::vector<BattedBall> balls = LoadBattedBalls(kFixturePath);

    REQUIRE(balls.size() == 3);

    REQUIRE(balls[0].gameDate == "2021-08-19");
    REQUIRE(balls[0].playerName == "Aaron Judge");
    REQUIRE(balls[0].batterId == 592450);
    REQUIRE(balls[0].launchSpeed == Catch::Approx(105.4));
    REQUIRE(balls[0].launchAngle == 27);
    REQUIRE(balls[0].events == "home_run");

    REQUIRE(balls[1].launchAngle == -15); // confirms negative angles parse correctly
}

TEST_CASE("LoadBattedBalls maps estimated_ba cells to nullopt when blank and a value when present", "[batted_ball]")
{
    // The fixture has 3 rows, one of which has a blank estimated_ba cell.
    // This test confirms that the blank cell is mapped to std::nullopt and that a non-blank cell is mapped to the correct value.
    std::vector<BattedBall> balls = LoadBattedBalls(kFixturePath);

    REQUIRE(balls[0].estimatedBa.has_value());
    REQUIRE(balls[0].estimatedBa.value() == Catch::Approx(0.850));

    REQUIRE_FALSE(balls[2].estimatedBa.has_value()); // blank cell in the fixture
}

TEST_CASE("LoadBattedBalls throws std::runtime_error when the file does not exist", "[batted_ball]")
{
    // LoadBattedBalls throws a std::runtime_error when specified CSV file does not exist.
    REQUIRE_THROWS_AS(LoadBattedBalls("no_such_file.csv"), std::runtime_error);
}
