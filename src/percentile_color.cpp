#include "percentile_color.hpp"

#include <algorithm>
#include <cmath>

namespace
{
    // The colors at percentile 0, 50 and 100.
    constexpr Rgb kLowColor = {50, 100, 200};     // blue
    constexpr Rgb kMiddleColor = {180, 180, 180}; // grey
    constexpr Rgb kHighColor = {215, 40, 40};     // red

    // Returns the value `fraction` of the way from start to end: 0 gives start, 0.5 is halfway, 1 gives end.
    int BlendChannel(int start, int end, double fraction)
    {
        return static_cast<int>(std::lround(start + (end - start) * fraction));
    }

    // Blends red, green and blue separately.
    Rgb BlendColor(Rgb start, Rgb end, double fraction)
    {
        return {BlendChannel(start.red, end.red, fraction),
                BlendChannel(start.green, end.green, fraction),
                BlendChannel(start.blue, end.blue, fraction)};
    }
} // namespace

Rgb PercentileColor(double percentile)
{
    const double clamped = std::clamp(percentile, 0.0, 100.0);

    // Each half of the range is its own blend. 90 is 80% of the way from 50 to 100,
    // so red, green and blue each move 80% of the way from grey to red.
    if (clamped <= 50.0)
    {
        return BlendColor(kLowColor, kMiddleColor, clamped / 50.0);
    }
    return BlendColor(kMiddleColor, kHighColor, (clamped - 50.0) / 50.0);
}
