#include "search_menu.hpp"

#include <iostream>
#include <string>

#include "pitcher_lookup.hpp"
#include "stat_card.hpp"

void RunSearchMenu(const std::vector<PitcherXba> &pitchers)
{
    std::string query;
    while (true)
    {
        std::cout << "\nPitcher name (Enter to quit): ";

        // Stop on an empty line, or if there's no more input at all (Ctrl+D).
        if (!std::getline(std::cin, query) || query.empty())
        {
            break;
        }

        const std::vector<PitcherXba> matches = FindPitchersByName(pitchers, query);
        if (matches.empty())
        {
            std::cout << "No pitcher matches \"" << query << "\".\n";
        }
        else if (matches.size() == 1)
        {
            std::cout << FormatStatCard(matches.front());
        }
        else
        {
            std::cout << matches.size() << " pitchers match. Type more of the name:\n";
            for (const PitcherXba &pitcher : matches)
            {
                std::cout << "  " << pitcher.pitcherName << "\n";
            }
        }
    }
}
