#pragma once

#include "domain/book/book.hpp"
#include "domain/common/id.hpp"

#include <optional>
#include <string_view>
#include <vector>

namespace bookshelf::application
{

// Ce dont le service a besoin pour ranger des livres. Il ne sait pas OÙ ils sont rangés :
// SQLite dans l'application, une simple liste en mémoire dans les tests.
class IBooks
{
public:
    IBooks() = default;
    IBooks(const IBooks&) = delete;
    IBooks& operator=(const IBooks&) = delete;
    virtual ~IBooks() = default;

    // Insère le livre (son `id` est ignoré) et renvoie l'identifiant attribué.
    [[nodiscard]] virtual domain::Id<domain::Book> create(const domain::Book& book) = 0;

    [[nodiscard]] virtual std::optional<domain::Book> get(domain::Id<domain::Book> id) = 0;

    // Faux si le livre n'existe pas.
    [[nodiscard]] virtual bool update(const domain::Book& book) = 0;

    // Faux si le livre n'existait pas.
    [[nodiscard]] virtual bool remove(domain::Id<domain::Book> id) = 0;

    // Livres dont le titre ou l'auteur contient `search` (tous si vide), triés par titre.
    [[nodiscard]] virtual std::vector<domain::Book> list(std::string_view search) = 0;
};

} // namespace bookshelf::application
