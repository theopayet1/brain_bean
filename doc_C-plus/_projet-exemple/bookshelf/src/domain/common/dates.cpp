#include "domain/common/dates.hpp"

#include <charconv>
#include <format>

namespace bookshelf::domain
{

std::string formatIso(Date date)
{
    return std::format("{:%F}", std::chrono::sys_days{date});
}

namespace
{

// Lit exactement `text.size()` chiffres. Faux s'il y a autre chose.
bool readNumber(std::string_view text, int& value)
{
    const auto* end = text.data() + text.size();
    const auto [stop, error] = std::from_chars(text.data(), end, value);
    return error == std::errc{} && stop == end;
}

} // namespace

std::optional<Date> parseIso(std::string_view text)
{
    // AAAA-MM-JJ : 10 caractères, des tirets aux positions 4 et 7.
    if (text.size() != 10 || text[4] != '-' || text[7] != '-')
    {
        return std::nullopt;
    }
    int year = 0;
    int month = 0;
    int day = 0;
    if (!readNumber(text.substr(0, 4), year) || !readNumber(text.substr(5, 2), month) ||
        !readNumber(text.substr(8, 2), day))
    {
        return std::nullopt;
    }
    const Date date{std::chrono::year{year},
                    std::chrono::month{static_cast<unsigned>(month)},
                    std::chrono::day{static_cast<unsigned>(day)}};
    // ok() refuse le 30 février, le mois 13…
    if (!date.ok())
    {
        return std::nullopt;
    }
    return date;
}

} // namespace bookshelf::domain
