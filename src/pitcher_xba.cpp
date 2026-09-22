#include "pitcher_xba.hpp"

#include <algorithm>
#include <unordered_map>

namespace
{
    // Running totals for one pitcher, while we're still going through their batted balls.
    struct PitcherAccumulator
    {
        std::string pitcherName;
        double hitRateSum = 0.0;
        int battedBallCount = 0;
    };
} // namespace

std::vector<PitcherXba> ComputePitcherXba(const std::vector<BattedBall> &battedBalls,
                                          const std::map<BucketKey, BucketStats> &buckets)
{
    std::unordered_map<long, PitcherAccumulator> accumulators;

    for (const BattedBall &ball : battedBalls)
    {
        const BucketKey bucketKey = BucketFor(ball.launchSpeed, ball.launchAngle);
        // .at() throws an error if bucket doesn't exist, instead of [] which would create a new bucket with 0 hits and 0 total
        const double hitRate = buckets.at(bucketKey).HitRate();

        PitcherAccumulator &acc = accumulators[ball.pitcherId];
        acc.pitcherName = ball.pitcherName;
        acc.hitRateSum += hitRate;
        ++acc.battedBallCount;
    }

    // Turn each pitcher's running total into a real xbaAllowed (sum / count).
    // percentile starts out empty since AssignPercentiles fills that in later.
    std::vector<PitcherXba> pitchers;
    pitchers.reserve(accumulators.size());
    for (const auto &[pitcherId, acc] : accumulators)
    {
        pitchers.push_back(PitcherXba{pitcherId, acc.pitcherName, acc.battedBallCount,
                                      acc.hitRateSum / acc.battedBallCount, std::nullopt});
    }
    return pitchers;
}

double PercentileRank(double value, const std::vector<double> &sortedValues)
{
    if (sortedValues.empty())
    {
        return 0.0;
    }

    // lower_bound finds the first value >= value; everything before it is smaller.
    // first element not less than value
    const auto firstNotLess = std::lower_bound(sortedValues.begin(), sortedValues.end(), value);
    // count of values less than value is the distance from the beginning of the vector to that first element not less than value.
    const auto countLess = std::distance(sortedValues.begin(), firstNotLess);

    return 100.0 * static_cast<double>(countLess) / static_cast<double>(sortedValues.size());
}

std::vector<PitcherXba> AssignPercentiles(const std::vector<PitcherXba> &pitchers, int minBattedBalls)
{
    // Only pitchers with enough batted balls count toward the comparison pool.
    std::vector<double> qualifiedXba;
    for (const PitcherXba &pitcher : pitchers)
    {
        if (pitcher.battedBallCount >= minBattedBalls)
        {
            qualifiedXba.push_back(pitcher.xbaAllowed);
        }
    }
    std::sort(qualifiedXba.begin(), qualifiedXba.end());

    // Copy pitchers so this function doesn't change the list it was given,
    // then fill in percentile for whoever qualified.
    std::vector<PitcherXba> result = pitchers;
    for (PitcherXba &pitcher : result)
    {
        if (pitcher.battedBallCount >= minBattedBalls)
        {
            // Flipped: a low xbaAllowed is good pitching, so it should land at the high percentile.
            pitcher.percentile = 100.0 - PercentileRank(pitcher.xbaAllowed, qualifiedXba);
        }
    }
    return result;
}
