#pragma once

#include <string>

#include "pitcher_xba.hpp"

// Turns one pitcher's numbers into a short, readable card
//
//   Skenes, Paul (id 694973)
//     xBA allowed:   .215
//     Batted balls:  84
//     Percentile:    92nd
//
// A pitcher with too few batted balls gets "n/a" instead of a percentile.
std::string FormatStatCard(const PitcherXba &pitcher);
