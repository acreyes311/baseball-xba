#pragma once

#include <string>

// Draws a percentile as a bar of blocks: 92 becomes ██████████████████░░ (18 of 20 filled).
// Percentiles below 0 or above 100 are treated as 0 or 100.
std::string PercentileBar(double percentile, int width = 20);
