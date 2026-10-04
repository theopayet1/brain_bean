#pragma once

#include <string>
#include <string_view>

namespace bookshelf::domain
{

// Retire les espaces au début et à la fin.
[[nodiscard]] std::string trim(std::string_view text);

} // namespace bookshelf::domain
