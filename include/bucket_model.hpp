#pragma once

#include <map>
#include <utility>
#include <vector>

#include "batted_ball.hpp"

// Bigger buckets mean more batted balls per bucket (a more trustworthy hit
// rate) but lump together swings that don't really behave the same way.
// 5 mph × 5 degrees a middle-ground
constexpr double kVelocityBucketWidth = 5.0; // mph
constexpr int kAngleBucketWidth = 5;         // degrees

// Identifies one (exit velocity, launch angle) bucket.
// first = velocity bucket index, second = angle bucket index.
using BucketKey = std::pair<int, int>;

// Tracks how often batted balls in one bucket turned into a hit.
struct BucketStats
{
    int hits = 0;
    int total = 0;

    double HitRate() const;
};

// Maps a batted ball's raw (launch speed, launch angle) down to the bucket
// it belongs in.
BucketKey BucketFor(double launchSpeed, int launchAngle);

// For every bucket, counts how many batted balls landed in it and how
// many of those were hits.
std::map<BucketKey, BucketStats> BuildBucketModel(const std::vector<BattedBall> &battedBalls);
