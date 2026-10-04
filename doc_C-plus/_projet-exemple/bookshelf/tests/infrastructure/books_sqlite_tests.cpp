#include "infrastructure/sqlite/books_sqlite.hpp"
#include "infrastructure/sqlite/migrations.hpp"
#include "support/temp_database.hpp"

#include <doctest/doctest.h>

using namespace bookshelf;
using namespace bookshelf::infrastructure::sqlite;

TEST_CASE("un livre enregistré se relit à l'identique")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    BooksSqlite books{connection};

    const auto id = books.create({
        .title = "Dune",
        .author = "Frank Herbert",
        .year = 1965,
        .addedOn = std::chrono::year{2026} / 10 / 4,
    });
    const auto read = books.get(id);

    REQUIRE(read.has_value());
    CHECK(read->title == "Dune");
    CHECK(read->year == 1965);
    CHECK(read->addedOn == std::chrono::year{2026} / 10 / 4);
}

TEST_CASE("la recherche trouve par titre ou par auteur")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    BooksSqlite books{connection};
    (void)books.create({.title = "Dune", .author = "Frank Herbert"});
    (void)books.create({.title = "Fondation", .author = "Isaac Asimov"});

    CHECK(books.list("").size() == 2);
    CHECK(books.list("Asimov").size() == 1);
    CHECK(books.list("Dun").front().title == "Dune");
}

TEST_CASE("migrer deux fois ne casse rien")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    CHECK_NOTHROW(migrate(connection));
}

TEST_CASE("une transaction non validée est annulée")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    BooksSqlite books{connection};
    {
        Transaction transaction{connection};
        (void)books.create({.title = "Dune"});
        // pas de commit() : le destructeur annule
    }
    CHECK(books.list("").empty());
}
