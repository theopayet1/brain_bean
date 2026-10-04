#pragma once

#include "application/ports/books.hpp"
#include "application/ports/clock.hpp"
#include "domain/book/book.hpp"
#include "domain/common/result.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace bookshelf::application
{

// Ce que l'utilisateur saisit pour ajouter un livre.
struct NewBook
{
    std::string title;
    std::string author;
    std::optional<int> year;
};

// Les cas d'usage autour des livres : lister, ajouter, marquer comme lu, supprimer.
class BookService
{
public:
    BookService(IBooks& books, const IClock& clock);

    [[nodiscard]] std::vector<domain::Book> list(std::string_view search);

    [[nodiscard]] domain::Result<domain::Id<domain::Book>> add(NewBook request);

    [[nodiscard]] domain::Result<void> markAsRead(domain::Id<domain::Book> id, bool read);

    [[nodiscard]] domain::Result<void> remove(domain::Id<domain::Book> id);

private:
    IBooks& books_;
    const IClock& clock_;
};

} // namespace bookshelf::application
