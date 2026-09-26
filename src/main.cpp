#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "batted_ball.hpp"
#include "bucket_model.hpp"
#include "pitcher_xba.hpp"
#include "search_menu.hpp"

// Relative to build/, where the program is run from.
const std::string kDataPath = "../data/statcast_june2026.csv";

int main()
{
    std::vector<BattedBall> battedBalls;
    try
    {
        battedBalls = LoadBattedBalls(kDataPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load batted-ball data: " << e.what() << "\n";
        return 1;
    }

    // Do all the math once so every search below is instant.
    const std::map<BucketKey, BucketStats> buckets = BuildBucketModel(battedBalls);
    const std::vector<PitcherXba> pitcherXbas = ComputePitcherXba(battedBalls, buckets);
    const std::vector<PitcherXba> pitchers = AssignPercentiles(pitcherXbas);

    RunSearchMenu(pitchers);

    return 0;
}
