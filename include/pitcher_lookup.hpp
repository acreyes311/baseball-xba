#pragma once

#include <string>
#include <vector>

#include "pitcher_xba.hpp"

// Returns every pitcher whose name contains query, ignoring upper/lower case
// (e.g. "skenes" matches "Skenes, Paul"). Empty if nothing matches.
std::vector<PitcherXba> FindPitchersByName(const std::vector<PitcherXba> &pitchers,
                                           const std::string &query);
