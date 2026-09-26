#include "search_menu.hpp"

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <optional>
#include <string>
#include <tuple>

#include "cli_input.hpp"
#include "pitcher_lookup.hpp"
#include "stat_card.hpp"

namespace
{
    // Shows a numbered list of matches and asks the user to pick one.
    // Nothing if they don't enter one of the numbers.
    std::optional<PitcherXba> ChooseFromMenu(std::vector<PitcherXba> matches)
    {
        // Sort by name so the list is easy to scan. Two pitchers can share a
        // name, so fall back to id to keep the order the same on every run.
        std::sort(matches.begin(), matches.end(),
                  [](const PitcherXba &a, const PitcherXba &b)
                  { return std::tie(a.pitcherName, a.pitcherId) < std::tie(b.pitcherName, b.pitcherId); });

        std::cout << matches.size() << " pitchers match:\n";
        for (std::size_t i = 0; i < matches.size(); ++i)
        {
            std::cout << "  " << (i + 1) << ") " << matches[i].pitcherName
                      << " (id " << matches[i].pitcherId << ")\n";
        }
        std::cout << "Pick one (1-" << matches.size() << "): ";

        std::string choice;
        if (!std::getline(std::cin, choice))
        {
            return std::nullopt;
        }

        const std::optional<std::size_t> index = ParseMenuChoice(choice, matches.size());
        if (!index.has_value())
        {
            std::cout << "\"" << choice << "\" isn't one of the choices.\n";
            return std::nullopt;
        }
        return matches[*index];
    }
} // namespace

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
            const std::optional<PitcherXba> chosen = ChooseFromMenu(matches);
            if (chosen.has_value())
            {
                std::cout << "\n" << FormatStatCard(*chosen);
            }
        }
    }
}
