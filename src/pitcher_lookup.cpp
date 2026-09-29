#include "pitcher_lookup.hpp"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace
{
    // Returns a lowercase copy of text, so names can be compared without caring about case
    std::string ToLower(std::string text)
    {
        for (char &c : text)
        {
            // std::tolower needs an unsigned char: a plain char holding a non-English
            // letter (like the ñ in "Peña") can be negative, which std::tolower doesn't allow
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return text;
    }

    // An accented letter and the plain letter it turns into for searching
    struct AccentedLetter
    {
        const char *accented;
        char plain;
    };

    // Every accented letter in the Statcast names. In UTF-8 each one takes 2 bytes
    // so it doesn't fit in a single char and is stored as a string
    constexpr AccentedLetter kAccentedLetters[] = {
        {"á", 'a'}, {"é", 'e'}, {"í", 'i'}, {"ó", 'o'}, {"ú", 'u'}, {"ñ", 'n'},
    };

    // Returns a copy of text with each accented letter replaced by its plain letter,
    // so "sanchez" can match "Sánchez"
    std::string RemoveAccents(std::string text)
    {
        for (const AccentedLetter &letter : kAccentedLetters)
        {
            const std::string accented = letter.accented;

            // Replace every place this letter appears, searching again after each replacement
            std::size_t position = text.find(accented);
            while (position != std::string::npos)
            {
                text.replace(position, accented.size(), 1, letter.plain);
                position = text.find(accented, position + 1);
            }
        }
        return text;
    }

    // Splits text into its words ("  paul   skenes " becomes {"paul", "skenes"})
    std::vector<std::string> SplitWords(const std::string &text)
    {
        // >> reads one word at a time and skips any spaces around it
        std::istringstream stream(text);
        std::vector<std::string> words;
        std::string word;
        while (stream >> word)
        {
            words.push_back(word);
        }
        return words;
    }
} // namespace

std::vector<PitcherXba> FindPitchersByName(const std::vector<PitcherXba> &pitchers,
                                           const std::string &query)
{
    std::vector<PitcherXba> matches;

    // No words (an empty query, or only spaces) would match every name, so treat it as matching nothing
    const std::vector<std::string> words = SplitWords(RemoveAccents(ToLower(query)));
    if (words.empty())
    {
        return matches;
    }

    for (const PitcherXba &pitcher : pitchers)
    {
        const std::string lowerName = RemoveAccents(ToLower(pitcher.pitcherName));

        // A pitcher matches only if every word shows up somewhere in their name
        const bool allWordsFound = std::all_of(words.begin(), words.end(),
                                               [&lowerName](const std::string &word)
                                               { return lowerName.find(word) != std::string::npos; });
        if (allWordsFound)
        {
            matches.push_back(pitcher);
        }
    }
    return matches;
}
