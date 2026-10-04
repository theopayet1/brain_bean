---
tags:
  - projet/cpp
  - type/guide
  - techno/cpp
  - techno/glaze
  - sujet/ui
  - sujet/erreurs
  - statut/a-jour
aliases:
  - Bridge C++
cree: 2026-10-04
maj: 2026-10-04
---

# Le pont côté C++

> [!abstract] En une phrase
> La classe **`Bridge`** reçoit un texte JSON et renvoie un texte JSON. Elle lit l'enveloppe, cherche la fonction dans une **table**, lit la demande dans un **DTO** avec **glaze**, appelle le **service**, et écrit la réponse. Elle ne connaît ni la fenêtre ni la base : on la teste avec de simples chaînes. À lire avant : [[05 Le pont C++ JavaScript — le protocole]].

---

## 📄 Les DTO : `src/bridge/dto.hpp`

```cpp
#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// La forme des messages échangés avec l'interface. Le nom de chaque membre EST la clé JSON :
// le renommer change le contrat avec le frontend (frontend/src/bridge/types.ts).
namespace bookshelf::bridge::dto
{

struct Empty
{
};

struct Failure
{
    std::uint64_t id = 0;
    bool ok = false;
    std::string code;
    std::string message; // phrase lisible, jamais de détail technique
};

struct Book
{
    std::int64_t id = 0;
    std::string title;
    std::string author;
    std::optional<int> year;
    bool read = false;
    std::string addedOn; // « AAAA-MM-JJ »
};

struct ListBooksRequest
{
    std::string search;
};

struct AddBookRequest
{
    std::string title;
    std::string author;
    std::optional<int> year;
};

struct MarkAsReadRequest
{
    std::int64_t id = 0;
    bool read = false;
};

struct IdRequest
{
    std::int64_t id = 0;
};

struct Created
{
    std::int64_t id = 0;
};

} // namespace bookshelf::bridge::dto
```

> [!info] Définition — DTO
> Un **DTO** (*Data Transfer Object*) est une struct qui décrit **exactement** la forme d'un message. glaze lit et écrit le JSON **à partir des noms des membres**, sans une ligne de code de plus ([[05 JSON avec glaze]]).

| Choix | Pourquoi |
|---|---|
| Des DTO **séparés** des structs du domaine | Le domaine peut évoluer sans casser le contrat JSON, et inversement |
| `std::optional<int> year` | Absent du JSON si vide ; facultatif à la lecture |
| `std::string addedOn` | Les dates voyagent en texte ISO |
| `Empty` | Réponse `{}` pour une action sans résultat |

---

## 📄 L'en-tête : `src/bridge/bridge.hpp`

```cpp
#pragma once

#include "application/books/book_service.hpp"
#include "domain/common/result.hpp"

#include <functional>
#include <string>
#include <string_view>

// Le pont : l'équivalent des contrôleurs d'une API web. Il traduit le JSON reçu en appel de
// service, et le résultat en JSON. AUCUNE règle métier ici.
//
// Il ne connaît pas la fenêtre : il reçoit un texte et en renvoie un. On peut donc le tester
// sans interface.
//
//   demande : { "id": 7, "function": "listBooks", "request": { "search": "" } }
//   réponse : { "id": 7, "ok": true, "data": [...] }
//          ou { "id": 7, "ok": false, "code": "book.not_found", "message": "..." }
namespace bookshelf::bridge
{

class Bridge
{
public:
    // Où écrire les erreurs techniques (fichier journal dans l'appli, rien dans les tests).
    using Log = std::function<void(std::string_view function, std::string_view detail)>;

    Bridge(application::BookService& books, Log log);

    // Ne lève JAMAIS : toute erreur devient une réponse `ok: false`.
    [[nodiscard]] std::string handle(std::string_view message) noexcept;

private:
    // Le JSON de `data`, ou une erreur.
    using Response = domain::Result<std::string>;
    using Handler = Response (Bridge::*)(std::string_view request);

    struct Entry
    {
        std::string_view name;
        Handler handler;
    };

    [[nodiscard]] static const Entry* find(std::string_view name);

    Response listBooks(std::string_view request);
    Response addBook(std::string_view request);
    Response markAsRead(std::string_view request);
    Response removeBook(std::string_view request);

    application::BookService& books_;
    Log log_;
};

} // namespace bookshelf::bridge
```

| Choix | Pourquoi |
|---|---|
| `handle(std::string_view) noexcept` | Une seule entrée, qui ne lève jamais |
| `using Handler = Response (Bridge::*)(std::string_view)` | Un **pointeur vers une méthode** : la table associe un nom à une méthode |
| `Log` = `std::function` | Le pont journalise sans savoir où (fichier dans l'appli, chaîne dans les tests) |

---

## 📄 L'implémentation : `src/bridge/bridge.cpp`

```cpp
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
```

### Lecture guidée

| Partie | Rôle |
|---|---|
| `envelope::Request` avec `glz::raw_json request` | On lit l'enveloppe **sans** interpréter `request` : chaque fonction sait quelle forme elle attend |
| `Strict` | `error_on_unknown_keys` + `error_on_missing_keys` : lecture stricte |
| `read<T>()` | Un template qui lit n'importe quel DTO et renvoie `Result<T>` |
| `find()` + `static constexpr auto Functions` | La **liste fermée**. Ajouter une fonction = une ligne ici |
| `(this->*entry->handler)(...)` | Appelle la méthode pointée sur **cet** objet |
| `std::format(R"({{"id":{},"ok":true,"data":{}}})", ...)` | On colle le JSON de `data` déjà écrit : pas de double sérialisation |
| `catch (const std::exception&)` | L'imprévu : au journal, réponse générique |
| `FallbackResponse` | Si même l'écriture de l'erreur échoue (mémoire épuisée) : un JSON écrit à la main |

### Le motif d'une fonction exposée

Toutes suivent le même plan en 3 temps :

```cpp
Bridge::Response Bridge::addBook(std::string_view request)
{
    auto read = bridge::read<dto::AddBookRequest>(request);    // 1. lire
    if (!read)
    {
        return std::unexpected(read.error());
    }
    const auto id = books_.add({ /* DTO → demande du service */ });   // 2. appeler
    if (!id)
    {
        return std::unexpected(id.error());
    }
    return write(dto::Created{.id = id->value});                // 3. écrire
}
```

---

## 📄 Les messages : `src/bridge/messages.cpp`

```cpp
#include "bridge/messages.hpp"

#include "domain/common/errors.hpp"

#include <array>
#include <utility>

namespace bookshelf::bridge
{

namespace errors = domain::errors;

std::string_view messageFor(std::string_view code)
{
    static constexpr auto Messages = std::to_array<std::pair<std::string_view, std::string_view>>({
        {errors::BookNotFound, "Ce livre n'existe plus."},
        {errors::BookTitleMissing, "Le titre est obligatoire."},
        {errors::BookTitleTooLong, "Le titre est trop long (200 caractères au plus)."},
        {errors::BookYearInvalid, "L'année doit être comprise entre 1400 et 2100."},
    });
    for (const auto& [known, message] : Messages)
    {
        if (known == code)
        {
            return message;
        }
    }
    return "Une erreur inattendue s'est produite.";
}

} // namespace bookshelf::bridge
```

Un seul fichier pour toutes les phrases montrées à l'utilisateur : facile à relire, à corriger, à traduire.

---

## ➕ Exposer une nouvelle fonction

1. Le **service** a la méthode (`BookService::rename`).
2. **DTO** de la demande dans `dto.hpp` (`RenameBookRequest`).
3. Méthode privée `Response renameBook(std::string_view)` dans `bridge.hpp` / `.cpp`.
4. Une ligne dans `Functions` : `{"renameBook", &Bridge::renameBook}`.
5. Un message dans `messages.cpp` pour chaque nouveau code d'erreur.
6. Un test dans `tests/bridge/` ([[04 Tester le pont]]).
7. Côté TypeScript : le type dans `types.ts`, la fonction dans `api.ts` ([[07 Le pont côté TypeScript]]).

---

## 🔗 Liens

- [[05 JSON avec glaze]] — glaze en détail
- [[04 Tester le pont]] — les tests du pont
- [[07 Le pont côté TypeScript]] — la suite
