#pragma once

#include <chrono>

namespace bookshelf::application
{

// La date du jour. Derrière une interface pour que les tests puissent la fixer.
class IClock
{
public:
    IClock() = default;
    IClock(const IClock&) = delete;
    IClock& operator=(const IClock&) = delete;
    virtual ~IClock() = default;

    [[nodiscard]] virtual std::chrono::year_month_day today() const = 0;
};

} // namespace bookshelf::application
