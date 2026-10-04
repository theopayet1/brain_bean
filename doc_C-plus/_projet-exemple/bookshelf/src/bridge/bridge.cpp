#include "bridge/bridge.hpp"

#include "bridge/dto.hpp"
#include "bridge/messages.hpp"
#include "domain/common/dates.hpp"
#include "domain/common/errors.hpp"

#include <glaze/json.hpp>

#include <array>
#include <exception>
#include <format>
#include <stdexcept>
#include <utility>

namespace bookshelf::bridge
{

// L'enveloppe de chaque demande. Hors de l'espace de noms anonyme : la réflexion de glaze
// a besoin d'une liaison externe.
namespace envelope
{

struct Request
{
    std::uint64_t id = 0;
    std::string function;
    // Gardée en JSON brut : c'est la fonction appelée qui sait quelle forme elle attend.
    glz::raw_json request;
};

} // namespace envelope

namespace
{

using domain::fail;
namespace errors = domain::errors;

// Le frontend est le nôtre : une clé inconnue ou manquante est un bug, pas une tolérance.
// Les membres std::optional restent facultatifs.
constexpr glz::opts Strict{.error_on_unknown_keys = true, .error_on_missing_keys = true};

template <typename T>
domain::Result<T> read(std::string_view json)
{
    T value{};
    if (glz::read<Strict>(value, json))
    {
        return fail(errors::BridgeInvalidRequest);
    }
    return value;
}

template <typename T>
std::string write(const T& value)
{
    auto json = glz::write_json(value);
    if (!json)
    {
        throw std::logic_error("écriture JSON impossible");
    }
    return std::move(*json);
}

dto::Book toDto(domain::Book book)
{
    return {
        .id = book.id.value,
        .title = std::move(book.title),
        .author = std::move(book.author),
        .year = book.year,
        .read = book.read,
        .addedOn = domain::formatIso(book.addedOn),
    };
}

std::string failure(std::uint64_t id, std::string_view code)
{
    return write(dto::Failure{
        .id = id,
        .ok = false,
        .code = std::string{code},
        .message = std::string{messageFor(code)},
    });
}

// Dernier recours si même l'écriture de l'erreur échoue : du JSON écrit à la main.
constexpr std::string_view FallbackResponse =
    R"({"id":0,"ok":false,"code":"system.unexpected","message":"Une erreur inattendue s'est produite."})";

} // namespace

Bridge::Bridge(application::BookService& books, Log log)
    : books_(books)
    , log_(std::move(log))
{
}

const Bridge::Entry* Bridge::find(std::string_view name)
{
    // La liste COMPLÈTE de ce que le JavaScript peut demander. Le reste n'existe pas pour lui.
    static constexpr auto Functions = std::to_array<Entry>({
        {"listBooks", &Bridge::listBooks},
        {"addBook", &Bridge::addBook},
        {"markAsRead", &Bridge::markAsRead},
        {"removeBook", &Bridge::removeBook},
    });
    for (const Entry& entry : Functions)
    {
        if (entry.name == name)
        {
            return &entry;
        }
    }
    return nullptr;
}

std::string Bridge::handle(std::string_view message) noexcept
{
    std::uint64_t id = 0;
    std::string function;
    try
    {
        envelope::Request request;
        if (glz::read<Strict>(request, message))
        {
            return failure(0, errors::BridgeInvalidRequest);
        }
        id = request.id;
        function = request.function;

        const Entry* entry = find(request.function);
        const Response result = entry != nullptr ? (this->*entry->handler)(request.request.str)
                                                 : Response{fail(errors::BridgeUnknownFunction)};
        if (result)
        {
            return std::format(R"({{"id":{},"ok":true,"data":{}}})", id, *result);
        }
        if (!result.error().detail.empty())
        {
            log_(function, result.error().detail);
        }
        return failure(id, result.error().code);
    }
    catch (const std::exception& error)
    {
        // L'imprévu (disque plein, requête SQL fausse…) : au journal, jamais à l'écran.
        log_(function, error.what());
    }
    catch (...)
    {
        log_(function, "exception inconnue");
    }

    try
    {
        return failure(id, errors::Unexpected);
    }
    catch (...)
    {
        return std::string{FallbackResponse};
    }
}

// --- Les fonctions exposées : lire la demande, appeler le service, écrire la réponse ------

Bridge::Response Bridge::listBooks(std::string_view request)
{
    auto read = bridge::read<dto::ListBooksRequest>(request);
    if (!read)
    {
        return std::unexpected(read.error());
    }
    std::vector<dto::Book> books;
    for (domain::Book& book : books_.list(read->search))
    {
        books.push_back(toDto(std::move(book)));
    }
    return write(books);
}

Bridge::Response Bridge::addBook(std::string_view request)
{
    auto read = bridge::read<dto::AddBookRequest>(request);
    if (!read)
    {
        return std::unexpected(read.error());
    }
    const auto id = books_.add({
        .title = std::move(read->title),
        .author = std::move(read->author),
        .year = read->year,
    });
    if (!id)
    {
        return std::unexpected(id.error());
    }
    return write(dto::Created{.id = id->value});
}

Bridge::Response Bridge::markAsRead(std::string_view request)
{
    auto read = bridge::read<dto::MarkAsReadRequest>(request);
    if (!read)
    {
        return std::unexpected(read.error());
    }
    if (auto done = books_.markAsRead({read->id}, read->read); !done)
    {
        return std::unexpected(done.error());
    }
    return write(dto::Empty{});
}

Bridge::Response Bridge::removeBook(std::string_view request)
{
    auto read = bridge::read<dto::IdRequest>(request);
    if (!read)
    {
        return std::unexpected(read.error());
    }
    if (auto done = books_.remove({read->id}); !done)
    {
        return std::unexpected(done.error());
    }
    return write(dto::Empty{});
}

} // namespace bookshelf::bridge
