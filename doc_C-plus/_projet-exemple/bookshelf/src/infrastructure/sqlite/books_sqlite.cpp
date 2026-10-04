#include "infrastructure/sqlite/books_sqlite.hpp"

#include "domain/common/dates.hpp"

#include <format>

namespace bookshelf::infrastructure::sqlite
{

namespace
{

// Toujours les mêmes colonnes, dans le même ordre : readBook() lit par position.
constexpr std::string_view Columns = "id, title, author, year, read, added_on";

domain::Book readBook(const Statement& row)
{
    return {
        .id = {row.integer(0)},
        .title = row.text(1),
        .author = row.text(2),
        .year = row.integerOrNull(3).transform([](std::int64_t y) { return static_cast<int>(y); }),
        .read = row.integer(4) != 0,
        .addedOn = domain::parseIso(row.text(5)).value_or(domain::Date{}),
    };
}

} // namespace

BooksSqlite::BooksSqlite(Connection& connection)
    : connection_(connection)
{
}

domain::Id<domain::Book> BooksSqlite::create(const domain::Book& book)
{
    connection_
        .prepare("INSERT INTO book (title, author, year, read, added_on) VALUES (?, ?, ?, ?, ?)")
        .bind(book.title)
        .bind(book.author)
        .bind(book.year)
        .bind(book.read)
        .bind(domain::formatIso(book.addedOn))
        .run();
    return {connection_.lastInsertId()};
}

std::optional<domain::Book> BooksSqlite::get(domain::Id<domain::Book> id)
{
    auto statement = connection_.prepare(std::format("SELECT {} FROM book WHERE id = ?", Columns));
    statement.bind(id.value);
    if (!statement.next())
    {
        return std::nullopt;
    }
    return readBook(statement);
}

bool BooksSqlite::update(const domain::Book& book)
{
    connection_.prepare("UPDATE book SET title = ?, author = ?, year = ?, read = ? WHERE id = ?")
        .bind(book.title)
        .bind(book.author)
        .bind(book.year)
        .bind(book.read)
        .bind(book.id.value)
        .run();
    return connection_.changes() > 0;
}

bool BooksSqlite::remove(domain::Id<domain::Book> id)
{
    connection_.prepare("DELETE FROM book WHERE id = ?").bind(id.value).run();
    return connection_.changes() > 0;
}

std::vector<domain::Book> BooksSqlite::list(std::string_view search)
{
    // Le texte cherché est un PARAMÈTRE, jamais collé dans le SQL : pas d'injection SQL.
    auto statement = connection_.prepare(
        std::format("SELECT {} FROM book WHERE ?1 = '' OR title LIKE '%' || ?1 || '%' "
                    "OR author LIKE '%' || ?1 || '%' ORDER BY title COLLATE NOCASE",
                    Columns));
    statement.bind(search);

    std::vector<domain::Book> books;
    while (statement.next())
    {
        books.push_back(readBook(statement));
    }
    return books;
}

} // namespace bookshelf::infrastructure::sqlite
