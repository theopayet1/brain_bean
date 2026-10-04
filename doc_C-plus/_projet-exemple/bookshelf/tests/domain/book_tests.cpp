#include "domain/book/book.hpp"
#include "domain/common/errors.hpp"

#include <doctest/doctest.h>

using namespace bookshelf::domain;

TEST_CASE("un livre valide est accepté et nettoyé")
{
    const auto book = validate({.title = "  Dune  ", .author = "Frank Herbert", .year = 1965});

    REQUIRE(book.has_value());
    CHECK(book->title == "Dune");
}

TEST_CASE("un livre sans titre est refusé")
{
    const auto book = validate({.title = "   ", .author = "Anonyme"});

    REQUIRE_FALSE(book.has_value());
    CHECK(book.error().code == errors::BookTitleMissing);
}

TEST_CASE("une année absurde est refusée")
{
    CHECK(validate({.title = "Dune", .year = 12}).error().code == errors::BookYearInvalid);
    CHECK(validate({.title = "Dune", .year = 1965}).has_value());
    CHECK(validate({.title = "Dune", .year = std::nullopt}).has_value());
}
