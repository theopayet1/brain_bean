#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <utility>

namespace bookshelf::domain
{

// Échec prévu d'une opération (livre introuvable, titre vide…).
// `code` est stable : l'interface s'en sert pour choisir le message à afficher.
// `detail` est technique : il part au journal, jamais à l'écran.
struct Error
{
    std::string_view code;
    std::string detail;
};

// Soit une valeur T, soit une Error. Remplace « return null » et les exceptions prévues.
template <typename T>
using Result = std::expected<T, Error>;

// Raccourci pour écrire `return fail(errors::BookNotFound);`
[[nodiscard]] inline std::unexpected<Error> fail(std::string_view code, std::string detail = {})
{
    return std::unexpected(Error{.code = code, .detail = std::move(detail)});
}

} // namespace bookshelf::domain
