#include "pitcher_lookup.hpp"

#include <cctype>

namespace
{
    // Returns a lowercase copy of text, so names can be compared without caring about case.
    std::string ToLower(std::string text)
    {
        for (char &c : text)
        {
            // std::tolower needs an unsigned char: a plain char holding a non-English
            // letter (like the ñ in "Peña") can be negative, which std::tolower doesn't allow.
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return text;
    }
} // namespace

std::vector<PitcherXba> FindPitchersByName(const std::vector<PitcherXba> &pitchers,
                                           const std::string &query)
{
    std::vector<PitcherXba> matches;

    // An empty query would match every name, so treat it as matching nothing.
    if (query.empty())
    {
        return matches;
    }

    const std::string lowerQuery = ToLower(query);
    for (const PitcherXba &pitcher : pitchers)
    {
        if (ToLower(pitcher.pitcherName).find(lowerQuery) != std::string::npos)
        {
            matches.push_back(pitcher);
        }
    }
    return matches;
}
