#pragma once

#include <optional>
#include <string>
#include <vector>

// One league-wide batted-ball event: a single pitch that was put in play
// and ended a plate appearance (fouls are excluded upstream, in
// scripts/pull_statcast.py, since they have no plate-appearance outcome).
struct BattedBall {
    std::string gameDate;
    std::string playerName;
    long batterId;
    double launchSpeed;
    int launchAngle;
    std::string events;
    // Statcast's own xBA estimate for this batted ball. Absent for a small
    // number of outcomes (e.g. sac bunts) that its model doesn't cover.
    std::optional<double> estimatedBa;
};

// Reads batted-ball rows from a CSV produced by scripts/pull_statcast.py.
// Throws std::runtime_error if csvPath does not exist.
std::vector<BattedBall> LoadBattedBalls(const std::string& csvPath);
