#pragma once

#include <string>
#include <vector>

#include "pitcher_xba.hpp"

// Returns every pitcher whose name contains every word in query, in any order,
// ignoring upper/lower case ("skenes" and "paul skenes" both match
// "Skenes, Paul"). Empty if nothing matches, or if query has no words.
std::vector<PitcherXba> FindPitchersByName(const std::vector<PitcherXba> &pitchers,
                                           const std::string &query);
