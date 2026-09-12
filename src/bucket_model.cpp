#include "bucket_model.hpp"

#include <cmath>
#include <string>
#include <unordered_set>

double BucketStats::HitRate() const
{
    return total == 0 ? 0.0 : static_cast<double>(hits) / total;
}

// anonymous namespace, invisible outside this cpp
namespace
{
    // Integer division rounds toward zero, -3 / 5 gives 0 instead of the -1 we want
    // (angle -3 belongs in the same bucket as -4 and -5, not with 0 through 4).
    // This rounds the way we need for negative values.
    int FloorDiv(int value, int divisor)
    {
        int quotient = value / divisor;
        int remainder = value % divisor;
        if (remainder != 0 && ((remainder < 0) != (divisor < 0)))
        {
            --quotient;
        }
        return quotient;
    }

    // Every batted-ball outcome that counts as a hit for xBA purposes.
    // Everything else in the events column (outs, errors, sac flies,
    // fielder's choice, etc.) counts as not a hit.
    bool IsHit(const std::string &events)
    {
        static const std::unordered_set<std::string> kHitEvents = {
            "single", "double", "triple", "home_run"};
        return kHitEvents.count(events) > 0;
    }
} // namespace

BucketKey BucketFor(double launchSpeed, int launchAngle)
{
    const int velocityBucket = static_cast<int>(std::floor(launchSpeed / kVelocityBucketWidth));
    const int angleBucket = FloorDiv(launchAngle, kAngleBucketWidth);
    return {velocityBucket, angleBucket};
}

std::map<BucketKey, BucketStats> BuildBucketModel(const std::vector<BattedBall> &battedBalls)
{
    std::map<BucketKey, BucketStats> buckets;
    for (const BattedBall &ball : battedBalls)
    {
        BucketStats &stats = buckets[BucketFor(ball.launchSpeed, ball.launchAngle)];
        ++stats.total;
        if (IsHit(ball.events))
        {
            ++stats.hits;
        }
    }
    return buckets;
}
