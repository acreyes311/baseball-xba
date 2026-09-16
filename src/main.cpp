#include <algorithm>
#include <iomanip>
#include <iostream>

#include "batted_ball.hpp"
#include "bucket_model.hpp"

int main(int argc, char* argv[]) {
    const std::string csvPath = (argc > 1) ? argv[1] : "../data/statcast_june2026.csv";

    std::vector<BattedBall> battedBalls;
    try {
        battedBalls = LoadBattedBalls(csvPath);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load batted-ball data: " << e.what() << "\n";
        return 1;
    }

    std::cout << "Loaded " << battedBalls.size() << " batted balls from " << csvPath << "\n\n";

    const size_t previewCount = std::min<size_t>(5, battedBalls.size());
    for (size_t i = 0; i < previewCount; ++i) {
        const BattedBall& ball = battedBalls[i];
        std::cout << ball.playerName << " | " << ball.launchSpeed << " mph, "
                  << ball.launchAngle << " deg | " << ball.events << " | xBA: "
                  << (ball.estimatedBa ? std::to_string(*ball.estimatedBa) : "N/A") << "\n";
    }

    // Sort every batted ball into a (velocity, angle) bucket and count hits vs. total in each.
    const std::map<BucketKey, BucketStats> buckets = BuildBucketModel(battedBalls);
    std::cout << "\nBuilt " << buckets.size() << " exit-velocity/launch-angle buckets.\n";

    // std::map sorts by key only, not by value, so copy into a vector to sort by sample size.
    std::vector<std::pair<BucketKey, BucketStats>> sortedBuckets(buckets.begin(), buckets.end());
    std::sort(sortedBuckets.begin(), sortedBuckets.end(),
              [](const auto& a, const auto& b) { return a.second.total > b.second.total; });

    std::cout << "\nTop buckets by sample size:\n";
    const size_t topCount = std::min<size_t>(10, sortedBuckets.size());
    for (size_t i = 0; i < topCount; ++i) {
        const auto& [key, stats] = sortedBuckets[i];
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

    return 0;
}
