#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "batted_ball.hpp"
#include "bucket_model.hpp"
#include "pitcher_lookup.hpp"
#include "pitcher_xba.hpp"
#include "stat_card.hpp"

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

    std::string query;
    while (true)
    {
        std::cout << "\nPitcher name (Enter to quit): ";

        // Stop on an empty line, or if there's no more input at all (Ctrl+D).
        if (!std::getline(std::cin, query) || query.empty())
        {
            break;
        }

        const std::vector<PitcherXba> matches = FindPitchersByName(pitchers, query);
        if (matches.empty())
        {
            std::cout << "No pitcher matches \"" << query << "\".\n";
        }
        else if (matches.size() == 1)
        {
            std::cout << FormatStatCard(matches.front());
        }
        else
        {
            std::cout << matches.size() << " pitchers match. Type more of the name:\n";
            for (const PitcherXba &pitcher : matches)
            {
                std::cout << "  " << pitcher.pitcherName << "\n";
            }
        }
    }

    return 0;
}
