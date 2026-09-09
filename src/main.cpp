#include <iostream>

#include "batted_ball.hpp"

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

    return 0;
}
