#include "batted_ball.hpp"

#include <filesystem>
#include <stdexcept>

#include "rapidcsv.hpp"

std::vector<BattedBall> LoadBattedBalls(const std::string &csvPath)
{
    if (!std::filesystem::exists(csvPath))
    {
        throw std::runtime_error("CSV file not found: " + csvPath);
    }

    rapidcsv::Document doc(csvPath);

    const std::vector<std::string> gameDates = doc.GetColumn<std::string>("game_date");
    const std::vector<std::string> playerNames = doc.GetColumn<std::string>("player_name");
    const std::vector<long> batterIds = doc.GetColumn<long>("batter");
    const std::vector<double> launchSpeeds = doc.GetColumn<double>("launch_speed");
    const std::vector<int> launchAngles = doc.GetColumn<int>("launch_angle");
    const std::vector<std::string> events = doc.GetColumn<std::string>("events");

    // estimated_ba_using_speedangle is blank for some rows (e.g. sac bunts),
    // so it needs a custom conversion instead of the default numeric one,
    // which would fail trying to parse an empty string as a double.
    const rapidcsv::ConvFunc<std::optional<double>> toOptionalDouble =
        [](const std::string &str, std::optional<double> &val)
    {
        if (str.empty())
        {
            val = std::nullopt;
        }
        else
        {
            val = std::stod(str);
        }
    };
    const std::vector<std::optional<double>> estimatedBas =
        doc.GetColumn<std::optional<double>>("estimated_ba_using_speedangle", toOptionalDouble);

    const size_t rowCount = doc.GetRowCount();
    std::vector<BattedBall> battedBalls;
    battedBalls.reserve(rowCount);
    for (size_t i = 0; i < rowCount; ++i)
    {
        battedBalls.push_back(BattedBall{
            gameDates[i],
            playerNames[i],
            batterIds[i],
            launchSpeeds[i],
            launchAngles[i],
            events[i],
            estimatedBas[i],
        });
    }
    return battedBalls;
}
