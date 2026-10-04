#pragma once

#include "application/ports/clock.hpp"

namespace bookshelf::infrastructure
{

// La vraie horloge : la date du jour à l'heure locale du poste.
class SystemClock : public application::IClock
{
public:
    [[nodiscard]] std::chrono::year_month_day today() const override;
};

} // namespace bookshelf::infrastructure
