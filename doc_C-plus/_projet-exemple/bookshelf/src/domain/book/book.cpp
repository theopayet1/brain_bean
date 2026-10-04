#include "domain/book/book.hpp"

#include "domain/common/errors.hpp"
#include "domain/common/text.hpp"

namespace bookshelf::domain
{

Result<Book> validate(Book book)
{
    book.title = trim(book.title);
    book.author = trim(book.author);

    if (book.title.empty())
    {
        return fail(errors::BookTitleMissing);
    }
    if (book.title.size() > MaxTitleLength)
    {
        return fail(errors::BookTitleTooLong);
    }
    // L'imprimerie a environ 600 ans : une année plus ancienne est une faute de frappe.
    if (book.year && (*book.year < 1400 || *book.year > 2100))
    {
        return fail(errors::BookYearInvalid);
    }
    return book;
}

} // namespace bookshelf::domain
