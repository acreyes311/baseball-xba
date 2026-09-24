#pragma once

#include <cstddef>
#include <optional>
#include <string>

// Turns a menu choice typed by the user ("1" to optionCount) into a 0-based
// index into the list of options. Nothing if it isn't one of the choices,
// "0", a number past optionCount, "abc", "2abc" or " 2".
std::optional<std::size_t> ParseMenuChoice(const std::string &input, std::size_t optionCount);
