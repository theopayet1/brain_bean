#include "domain/common/text.hpp"

namespace bookshelf::domain
{

std::string trim(std::string_view text)
{
    constexpr std::string_view Spaces = " \t\r\n";
    const auto first = text.find_first_not_of(Spaces);
    if (first == std::string_view::npos)
    {
        return {};
    }
    const auto last = text.find_last_not_of(Spaces);
    return std::string{text.substr(first, last - first + 1)};
}

} // namespace bookshelf::domain
