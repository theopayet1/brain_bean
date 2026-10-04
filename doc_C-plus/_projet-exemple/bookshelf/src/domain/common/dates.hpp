#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>

namespace bookshelf::domain
{

using Date = std::chrono::year_month_day;

// « 2026-10-04 » : le format ISO, qui se trie bien et que tout le monde comprend.
[[nodiscard]] std::string formatIso(Date date);

// L'inverse. std::nullopt si le texte n'est pas une vraie date (« 2026-02-30 » compris).
[[nodiscard]] std::optional<Date> parseIso(std::string_view text);

} // namespace bookshelf::domain
