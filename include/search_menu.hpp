#pragma once

#include <vector>

#include "pitcher_xba.hpp"

// Keep asking for a pitcher name and print what it finds,
// until the user presses Enter on an empty line (or there's no more input).
// Reads from std::cin and prints to std::cout, so it's part of the program,
// not the xba_core library.
void RunSearchMenu(const std::vector<PitcherXba> &pitchers);
