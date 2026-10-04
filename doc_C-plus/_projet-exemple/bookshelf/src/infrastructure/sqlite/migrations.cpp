#include "infrastructure/sqlite/migrations.hpp"

#include <array>
#include <format>
#include <string_view>

namespace bookshelf::infrastructure::sqlite
{

namespace
{

// RÈGLE D'OR : on ne modifie JAMAIS une migration déjà livrée. On en ajoute une à la fin.
constexpr auto Migrations = std::to_array<std::string_view>({
    // 1 : schéma initial
    R"sql(
CREATE TABLE book (
  id       INTEGER PRIMARY KEY,
  title    TEXT NOT NULL CHECK (length(trim(title)) > 0),
  author   TEXT NOT NULL DEFAULT '',
  year     INTEGER,
  read     INTEGER NOT NULL DEFAULT 0 CHECK (read IN (0, 1)),
  added_on TEXT NOT NULL
) STRICT;
)sql",
    // 2 : la recherche par titre doit rester rapide avec beaucoup de livres
    R"sql(
CREATE INDEX book_title ON book (title);
)sql",
});

int currentVersion(Connection& connection)
{
    auto statement = connection.prepare("PRAGMA user_version;");
    statement.next();
    return static_cast<int>(statement.integer(0));
}

} // namespace

void migrate(Connection& connection)
{
    const int from = currentVersion(connection);
    for (int version = from + 1; version <= static_cast<int>(Migrations.size()); ++version)
    {
        // Chaque migration et le numéro de version passent ensemble, ou pas du tout.
        Transaction transaction{connection};
        connection.execute(Migrations[static_cast<std::size_t>(version - 1)]);
        connection.execute(std::format("PRAGMA user_version = {};", version));
        transaction.commit();
    }
}

} // namespace bookshelf::infrastructure::sqlite
