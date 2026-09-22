#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>

#include "batted_ball.hpp"
#include "bucket_model.hpp"
#include "pitcher_xba.hpp"

int main(int argc, char *argv[])
{
    const std::string csvPath = (argc > 1) ? argv[1] : "../data/statcast_june2026.csv";

    std::vector<BattedBall> battedBalls;
    try
    {
        battedBalls = LoadBattedBalls(csvPath);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load batted-ball data: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Loaded " << battedBalls.size() << " batted balls from " << csvPath << "\n\n";

    // Diagnostic: just checking the data loaded right.
    const size_t previewCount = std::min<size_t>(5, battedBalls.size());
    for (size_t i = 0; i < previewCount; ++i)
    {
        const BattedBall &ball = battedBalls[i];
        std::cout << ball.pitcherName << " | " << ball.launchSpeed << " mph, "
                  << ball.launchAngle << " deg | " << ball.events << " | xBA: "
                  << (ball.estimatedBa ? std::to_string(*ball.estimatedBa) : "N/A") << "\n";
    }

    // Sort every batted ball into a (velocity, angle) bucket and count hits vs. total in each.
    const std::map<BucketKey, BucketStats> buckets = BuildBucketModel(battedBalls);
    std::cout << "\nBuilt " << buckets.size() << " exit-velocity/launch-angle buckets.\n";

    // std::map sorts by key only, not by value, so copy into a vector to sort by sample size.
    std::vector<std::pair<BucketKey, BucketStats>> sortedBuckets(buckets.begin(), buckets.end());
    std::sort(sortedBuckets.begin(), sortedBuckets.end(),
              [](const auto &a, const auto &b)
              { return a.second.total > b.second.total; });

    // Diagnostic: checking the bucket model looks right.
    std::cout << "\nTop buckets by sample size:\n";
    const size_t topCount = std::min<size_t>(10, sortedBuckets.size());
    for (size_t i = 0; i < topCount; ++i)
    {
        const auto &[key, stats] = sortedBuckets[i];
        // key.first/second are just bucket indices (e.g. bucket 18), so multiply by the
        // bucket width to turn them back into a real mph/degree range for display.
        const int velocityLow = static_cast<int>(key.first * kVelocityBucketWidth);
        const int velocityWidth = static_cast<int>(kVelocityBucketWidth);
        const int angleLow = key.second * kAngleBucketWidth;
        std::cout << velocityLow << "-" << (velocityLow + velocityWidth) << " mph, "
                  << angleLow << "-" << (angleLow + kAngleBucketWidth) << " deg | "
                  << stats.total << " balls | " << std::fixed << std::setprecision(3)
                  << stats.HitRate() << " hit rate\n";
    }

    // Turn each pitcher's batted balls into one xbaAllowed, then rank qualified pitchers against each other.
    const std::vector<PitcherXba> pitcherXbas = ComputePitcherXba(battedBalls, buckets);
    const std::vector<PitcherXba> pitchers = AssignPercentiles(pitcherXbas);

    const auto qualifiedCount = std::count_if(pitchers.begin(), pitchers.end(),
                                              [](const PitcherXba &p)
                                              { return p.percentile.has_value(); });
    std::cout << "\nComputed xBA allowed for " << pitchers.size() << " pitchers ("
              << qualifiedCount << " meet the " << kMinBattedBallsForPercentile
              << "-batted-ball minimum for percentile ranking).\n";

    // Only rank qualified pitchers here, so a small-sample pitcher can't take a top/bottom spot.
    std::vector<PitcherXba> qualified;
    std::copy_if(pitchers.begin(), pitchers.end(), std::back_inserter(qualified),
                 [](const PitcherXba &p)
                 { return p.percentile.has_value(); });
    // Ascending: lowest xbaAllowed (best pitching) first.
    std::sort(qualified.begin(), qualified.end(),
              [](const auto &a, const auto &b)
              { return a.xbaAllowed < b.xbaAllowed; });

    // A small local function to print one pitcher's stat line.
    auto printPitcher = [](const PitcherXba &p)
    {
        std::cout << p.pitcherName << " | " << std::fixed << std::setprecision(3) << p.xbaAllowed
                  << " xBA allowed | " << p.battedBallCount << " balls | percentile: "
                  << static_cast<int>(std::round(*p.percentile)) << "\n";
    };

    // Diagnostic: will be replaced by a real pitcher lookup later.
    std::cout << "\nTop 10 qualified pitchers by xBA allowed (lowest = best):\n";
    for (size_t i = 0; i < std::min<size_t>(10, qualified.size()); ++i)
    {
        printPitcher(qualified[i]);
    }

    std::cout << "\nBottom 10 qualified pitchers by xBA allowed (highest = worst):\n";
    for (size_t i = qualified.size() - std::min<size_t>(10, qualified.size()); i < qualified.size(); ++i)
    {
        printPitcher(qualified[i]);
    }

    return 0;
}
