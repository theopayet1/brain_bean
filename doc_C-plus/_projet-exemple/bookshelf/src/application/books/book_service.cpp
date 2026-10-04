#include "application/books/book_service.hpp"

#include "domain/common/errors.hpp"

#include <utility>

namespace bookshelf::application
{

using domain::fail;
namespace errors = domain::errors;

BookService::BookService(IBooks& books, const IClock& clock)
    : books_(books)
    , clock_(clock)
{
}

std::vector<domain::Book> BookService::list(std::string_view search)
{
    return books_.list(search);
}

domain::Result<domain::Id<domain::Book>> BookService::add(NewBook request)
{
    auto book = domain::validate({
        .title = std::move(request.title),
        .author = std::move(request.author),
        .year = request.year,
        .addedOn = clock_.today(),
    });
    if (!book)
    {
        return std::unexpected(book.error());
    }
    return books_.create(*book);
}

domain::Result<void> BookService::markAsRead(domain::Id<domain::Book> id, bool read)
{
    auto book = books_.get(id);
    if (!book)
    {
        return fail(errors::BookNotFound);
    }
    book->read = read;
    if (!books_.update(*book))
    {
        return fail(errors::BookNotFound);
    }
    return {};
}

domain::Result<void> BookService::remove(domain::Id<domain::Book> id)
{
    if (!books_.remove(id))
    {
        return fail(errors::BookNotFound);
    }
    return {};
}

} // namespace bookshelf::application
