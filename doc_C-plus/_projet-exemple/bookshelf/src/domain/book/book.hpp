#pragma once

#include "domain/common/dates.hpp"
#include "domain/common/id.hpp"
#include "domain/common/result.hpp"

#include <optional>
#include <string>

namespace bookshelf::domain
{

// Un livre de la bibliothèque. Une struct « bête » : des données, pas de base, pas d'écran.
struct Book
{
    Id<Book> id;
    std::string title;
    std::string author;
    std::optional<int> year; // absent : année inconnue
    bool read = false;
    Date addedOn{};
};

inline constexpr std::size_t MaxTitleLength = 200;

// Vérifie les règles d'un livre et le renvoie nettoyé (espaces retirés).
[[nodiscard]] Result<Book> validate(Book book);

} // namespace bookshelf::domain
