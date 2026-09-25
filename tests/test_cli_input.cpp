#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

#include "cli_input.hpp"

// Valid choice test
TEST_CASE("ParseMenuChoice turns a menu number into a 0-based index", "[cli_input]")
{
    // First, middle and last option of a 3-option menu.
    const std::optional<std::size_t> first = ParseMenuChoice("1", 3);
    REQUIRE(first.has_value());
    REQUIRE(*first == 0);

    const std::optional<std::size_t> middle = ParseMenuChoice("2", 3);
    REQUIRE(middle.has_value());
    REQUIRE(*middle == 1);

    const std::optional<std::size_t> last = ParseMenuChoice("3", 3);
    REQUIRE(last.has_value());
    REQUIRE(*last == 2);
}

// Out of range test
TEST_CASE("ParseMenuChoice rejects numbers that aren't on the menu", "[cli_input]")
{
    // The menu starts at 1, and a 3-option menu has no 4th option.
    REQUIRE_FALSE(ParseMenuChoice("0", 3).has_value());
    REQUIRE_FALSE(ParseMenuChoice("4", 3).has_value());
}

// Not a number test
TEST_CASE("ParseMenuChoice rejects input that isn't a whole number", "[cli_input]")
{
    const std::vector<std::string> badInputs = {
        "",      // just pressing Enter
        "abc",   // letters
        "2abc",  // a number with junk after it
        " 2",    // a space before the number
        "2 ",    // a space after the number
        "-1",    // negative
        "1.5",   // not a whole number
    };

    for (const std::string &input : badInputs)
    {
        // CAPTURE prints the input if a check fails, so we know which case broke.
        CAPTURE(input);
        REQUIRE_FALSE(ParseMenuChoice(input, 3).has_value());
    }
}

// Too big test
TEST_CASE("ParseMenuChoice rejects a number too big to fit in a long", "[cli_input]")
{
    // 20 digits is bigger than a long can hold, so from_chars reports an error
    // instead of wrapping around to some other number.
    REQUIRE_FALSE(ParseMenuChoice("99999999999999999999", 3).has_value());
}
