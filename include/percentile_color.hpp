#pragma once

// A color as amounts of red, green and blue, each from 0 to 255.
struct Rgb
{
    int red;
    int green;
    int blue;
};

// Picks a percentile's color, blending smoothly like Baseball Savant: blue at 0, grey at 50, red at 100.
// Percentiles below 0 or above 100 are treated as 0 or 100.
Rgb PercentileColor(double percentile);
