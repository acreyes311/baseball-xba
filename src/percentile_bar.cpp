#include "percentile_bar.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    // Each block is several bytes in UTF-8, so they need to be stored as strings, not chars.
    const std::string kFilledBlock = "█";
    const std::string kEmptyBlock = "░";

    // Number of blocks in the bar, so each block stands for 5 percentile points.
    constexpr int kBarWidth = 20;
} // namespace

std::string PercentileBar(double percentile)
{
    // clamp() restricts the value to the range [0, 100], and lround rounds to the nearest integer.
    const double clamped = std::clamp(percentile, 0.0, 100.0);
    const long filled = std::lround(clamped / 100.0 * kBarWidth);

    // Fill the bar with filled and empty blocks based on the calculated filled length.
    std::string bar;
    for (int i = 0; i < kBarWidth; ++i)
    {
        if (i < filled)
        {
            bar += kFilledBlock;
        }
        else
        {
            bar += kEmptyBlock;
        }
    }
    return bar;
}
