#pragma once

#include <optional>
#include <string>
#include <vector>

#include "pitcher_xba.hpp"

// Returns the pitcher with this MLBAM id, or nothing if no pitcher has it.
std::optional<PitcherXba> FindPitcherById(const std::vector<PitcherXba> &pitchers, long pitcherId);

// Returns every pitcher whose name contains query, ignoring upper/lower case
// (e.g. "skenes" matches "Skenes, Paul"). Empty if nothing matches.
std::vector<PitcherXba> FindPitchersByName(const std::vector<PitcherXba> &pitchers,
                                           const std::string &query);
