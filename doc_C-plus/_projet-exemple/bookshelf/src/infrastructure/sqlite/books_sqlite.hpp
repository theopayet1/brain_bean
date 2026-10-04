#pragma once

#include "application/ports/books.hpp"
#include "infrastructure/sqlite/connection.hpp"

namespace bookshelf::infrastructure::sqlite
{

// Le port IBooks, rangé dans SQLite.
class BooksSqlite : public application::IBooks
{
public:
    explicit BooksSqlite(Connection& connection);

    domain::Id<domain::Book> create(const domain::Book& book) override;
    std::optional<domain::Book> get(domain::Id<domain::Book> id) override;
    bool update(const domain::Book& book) override;
    bool remove(domain::Id<domain::Book> id) override;
    std::vector<domain::Book> list(std::string_view search) override;

private:
    Connection& connection_;
};

} // namespace bookshelf::infrastructure::sqlite
