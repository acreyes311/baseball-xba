#include "percentile_bar.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    // Each block is several bytes in UTF-8, so they need to be stored as strings, not chars.
    const std::string kFilledBlock = "█";
    const std::string kEmptyBlock = "░";
} // namespace

std::string PercentileBar(double percentile, int width)
{
    // clamp() restricts the value to the range [0, 100], and lround rounds to the nearest integer.
    const double clamped = std::clamp(percentile, 0.0, 100.0);
    const long filled = std::lround(clamped / 100.0 * width);

    // Fill the bar with filled and empty blocks based on the calculated filled length.
    std::string bar;
    for (int i = 0; i < width; ++i)
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
