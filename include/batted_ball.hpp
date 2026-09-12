#pragma once

#include <optional>
#include <string>
#include <vector>

// A batted-ball event: a single pitch that was put in play
// and ended a plate appearance (fouls are excluded, in
// scripts/pull_statcast.py, since they have no plate-appearance outcome).
struct BattedBall
{
    std::string gameDate;
    std::string playerName;
    long batterId;
    double launchSpeed;
    int launchAngle;
    std::string events;
    // estimateBa; Statcast's own xBA estimate for this batted ball.
    // Doesn't cover a small number of outcomes (e.g. sac bunts).
    std::optional<double> estimatedBa;
};

// Reads batted-ball rows from a CSV (scripts/pull_statcast.py.)
// Throws std::runtime_error if csvPath does not exist.
std::vector<BattedBall> LoadBattedBalls(const std::string &csvPath);
