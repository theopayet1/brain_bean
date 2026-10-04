#pragma once

#include <compare>
#include <cstdint>

namespace bookshelf::domain
{

// Identifiant typé : un Id<Book> ne se confond pas avec un Id<Loan>.
template <typename T>
struct Id
{
    std::int64_t value = 0;

    friend constexpr auto operator<=>(Id, Id) = default;
};

} // namespace bookshelf::domain
