#include "cli_input.hpp"

#include <charconv>
#include <system_error>

namespace
{
    // Returns text as a number if the whole thing is a whole number of 0 or more
    // that fits in a long. Nothing for "", "abc", "12abc", " 5" or "-5".
    std::optional<long> ParseWholeNumber(const std::string &text)
    {
        const char *first = text.data();
        const char *last = text.data() + text.size();

        // from_chars reads digits from first toward last and writes the number into value.
        // It gives back where it stopped reading, and an error code if it couldn't read a number.
        long value = 0;
        const auto [stoppedAt, error] = std::from_chars(first, last, value);

        // error is set when text doesn't start with a number, or the number is too big for a long.
        // stoppedAt != last means something came after the number, like the "abc" in "12abc".
        if (error != std::errc() || stoppedAt != last || value < 0)
        {
            return std::nullopt;
        }
        return value;
    }
} // namespace

std::optional<std::size_t> ParseMenuChoice(const std::string &input, std::size_t optionCount)
{
    // Reject anything that isn't a number from 1 to optionCount.
    // choice < 1 is checked first, so the cast to size_t (which can't be negative) is safe.
    const std::optional<long> choice = ParseWholeNumber(input);
    if (!choice.has_value() || *choice < 1 || static_cast<std::size_t>(*choice) > optionCount)
    {
        return std::nullopt;
    }

    // The menu shown to the user starts at 1, but vector indexes start at 0.
    return static_cast<std::size_t>(*choice - 1);
}
