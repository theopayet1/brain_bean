#include "application/books/book_service.hpp"
#include "domain/common/errors.hpp"
#include "support/fakes.hpp"

#include <doctest/doctest.h>

using namespace bookshelf;
using bookshelf::tests::FakeBooks;
using bookshelf::tests::FixedClock;

TEST_CASE("ajouter un livre lui donne la date du jour")
{
    FakeBooks books;
    const FixedClock clock{std::chrono::year{2026} / 10 / 4};
    application::BookService service{books, clock};

    const auto id = service.add({.title = "Dune", .author = "Frank Herbert", .year = 1965});

    REQUIRE(id.has_value());
    const auto saved = books.get(*id);
    REQUIRE(saved.has_value());
    CHECK(saved->addedOn == std::chrono::year{2026} / 10 / 4);
    CHECK_FALSE(saved->read);
}

TEST_CASE("un livre invalide n'est jamais enregistré")
{
    FakeBooks books;
    const FixedClock clock;
    application::BookService service{books, clock};

    const auto id = service.add({.title = ""});

    CHECK(id.error().code == domain::errors::BookTitleMissing);
    CHECK(books.list("").empty());
}

TEST_CASE("marquer comme lu un livre qui n'existe pas renvoie une erreur")
{
    FakeBooks books;
    const FixedClock clock;
    application::BookService service{books, clock};

    const auto result = service.markAsRead({42}, true);

    CHECK(result.error().code == domain::errors::BookNotFound);
}
