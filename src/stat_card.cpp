#include "stat_card.hpp"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace
{
    // Formats an average the way baseball writes it: 0.2146 becomes ".215".
    std::string FormatAverage(double average)
    {
        std::ostringstream out;
        out << std::fixed << std::setprecision(3) << average;
        std::string text = out.str();

        // Drop the leading zero, so "0.215" becomes ".215".
        if (text.size() > 1 && text[0] == '0' && text[1] == '.')
        {
            text.erase(0, 1);
        }
        return text;
    }

    // Returns "st", "nd", "rd" or "th" for n, so it can be written as 1st, 2nd, 3rd, 4th, ...
    std::string OrdinalSuffix(long n)
    {
        // 11, 12 and 13 break the pattern: 11th, not 11st.
        const long lastTwoDigits = n % 100;
        if (lastTwoDigits >= 11 && lastTwoDigits <= 13)
        {
            return "th";
        }

        switch (n % 10)
        {
        case 1:
            return "st";
        case 2:
            return "nd";
        case 3:
            return "rd";
        default:
            return "th";
        }
    }
} // namespace

std::string FormatStatCard(const PitcherXba &pitcher)
{
    std::ostringstream card;
    card << pitcher.pitcherName << " (id " << pitcher.pitcherId << ")\n";
    card << "  xBA allowed:   " << FormatAverage(pitcher.xbaAllowed) << "\n";
    card << "  Batted balls:  " << pitcher.battedBallCount << "\n";

    card << "  Percentile:    ";
    if (pitcher.percentile.has_value())
    {
        const long rounded = std::lround(*pitcher.percentile);
        card << rounded << OrdinalSuffix(rounded) << "\n";
    }
    else
    {
        card << "n/a (needs " << kMinBattedBallsForPercentile << "+ batted balls)\n";
    }

    return card.str();
}
