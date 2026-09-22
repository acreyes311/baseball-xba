#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "batted_ball.hpp"
#include "bucket_model.hpp"

// Minimum is set to 20 as anything lower is too small to be meaningful.
constexpr int kMinBattedBallsForPercentile = 20;

// One pitcher's xBA allowed, and where it ranks against the league (if they have enough batted balls).
struct PitcherXba
{
    long pitcherId;
    std::string pitcherName;
    int battedBallCount;
    double xbaAllowed;
    std::optional<double> percentile; // unset if battedBallCount is too low
};

// Groups battedBalls by pitcher and averages each pitcher's bucket hit rates into one xbaAllowed.
std::vector<PitcherXba> ComputePitcherXba(const std::vector<BattedBall> &battedBalls,
                                           const std::map<BucketKey, BucketStats> &buckets);

// Of all the values in sortedValues, what percent are smaller than value.
// sortedValues must be sorted in ascending order.
double PercentileRank(double value, const std::vector<double> &sortedValues);

// Fills in percentile for every pitcher with enough batted balls, ranked against each other.
// Flipped so a low xbaAllowed (good pitching) lands at the high end.
std::vector<PitcherXba> AssignPercentiles(const std::vector<PitcherXba> &pitchers,
                                           int minBattedBalls = kMinBattedBallsForPercentile);
