#include "domain/common/dates.hpp"

#include <doctest/doctest.h>

using namespace bookshelf::domain;
using namespace std::chrono;

TEST_CASE("une date fait l'aller-retour en texte ISO")
{
    const Date date = year{2026} / 10 / 4;
    CHECK(formatIso(date) == "2026-10-04");
    CHECK(parseIso("2026-10-04") == date);
}

TEST_CASE("les fausses dates sont refusées")
{
    CHECK_FALSE(parseIso("2026-02-30").has_value());
    CHECK_FALSE(parseIso("04/10/2026").has_value());
    CHECK_FALSE(parseIso("").has_value());
    CHECK_FALSE(parseIso("2026-1a-04").has_value());
}
