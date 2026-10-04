#pragma once

#include <string_view>

// Codes d'erreur stables : une fois livrés, ils ne changent plus jamais de sens.
namespace bookshelf::domain::errors
{

inline constexpr std::string_view BookNotFound = "book.not_found";
inline constexpr std::string_view BookTitleMissing = "book.title_missing";
inline constexpr std::string_view BookTitleTooLong = "book.title_too_long";
inline constexpr std::string_view BookYearInvalid = "book.year_invalid";

inline constexpr std::string_view BridgeInvalidRequest = "bridge.invalid_request";
inline constexpr std::string_view BridgeUnknownFunction = "bridge.unknown_function";
inline constexpr std::string_view Unexpected = "system.unexpected";

} // namespace bookshelf::domain::errors
