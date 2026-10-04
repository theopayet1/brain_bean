#include "infrastructure/system/system_clock.hpp"

namespace bookshelf::infrastructure
{

std::chrono::year_month_day SystemClock::today() const
{
    // system_clock est en UTC : à 0 h 30 en France, la date UTC est encore la veille.
    // On passe donc par le fuseau du poste.
    const auto local = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(local)};
}

} // namespace bookshelf::infrastructure
