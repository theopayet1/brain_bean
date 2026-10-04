---
tags:
  - projet/cpp
  - type/archi
  - techno/cpp
  - sujet/architecture
  - sujet/injection-de-dependances
  - statut/a-jour
aliases:
  - Services C++
  - Cas d'usage
cree: 2026-10-04
maj: 2026-10-04
---

# Les services (cas d'usage)

> [!abstract] En une phrase
> Un **service** regroupe les **cas d'usage** d'un domaine fonctionnel (« ajouter un livre », « marquer comme lu ») : il reçoit ses ports par son **constructeur**, applique les règles du domaine, et renvoie un `Result`. C'est une classe **concrète** (pas d'interface) qui ne connaît ni la base, ni le JSON, ni la fenêtre.

---

## 📄 `BookService`

```cpp
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
```

```cpp
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
```

| Point | Pourquoi |
|---|---|
| `NewBook` | La **demande** : ce que l'utilisateur saisit, distinct de `Book` (pas d'`id`, pas de date d'ajout) |
| Membres `IBooks&` et `const IClock&` | **Injection par constructeur** : le service utilise, il ne crée pas |
| `add` appelle `validate` **avant** `create` | Rien d'invalide n'atteint la base |
| `addedOn = clock_.today()` | La date vient du port : testable avec une date fixe |
| `return std::unexpected(book.error())` | On transmet l'erreur du domaine telle quelle |
| `markAsRead` charge, modifie, enregistre | Le motif classique « lire → modifier → écrire » |

---

## 💉 L'injection de dépendances, sans conteneur

> [!info] Définition — injection de dépendances
> Un objet **reçoit** les objets dont il a besoin (par son constructeur) au lieu de les **créer** lui-même. Celui qui l'assemble choisit les implémentations : la vraie base dans l'appli, une fausse dans les tests.

```cpp
// Dans l'application (main) :
infrastructure::sqlite::BooksSqlite books{connection};
infrastructure::SystemClock clock;
application::BookService service{books, clock};

// Dans un test :
tests::FakeBooks books;
tests::FixedClock clock{std::chrono::year{2026} / 10 / 4};
application::BookService service{books, clock};
```

Pas de bibliothèque d'injection : en C++, l'assemblage à la main dans `main` est court, lisible, et vérifié par le compilateur. Voir [[06 La racine de composition]].

---

## 📏 Règles d'un bon service

| Règle | Pourquoi |
|---|---|
| Un service **par domaine fonctionnel** (`BookService`, `LoanService`) | Pas un « god service » qui fait tout |
| Une méthode = **un cas d'usage complet** | Le pont appelle une méthode et c'est fini |
| Les règles sont dans le **domaine**, le service les **orchestre** | `validate()` est dans le domaine, `add()` dans le service |
| Plusieurs écritures → **une transaction** | Tout ou rien ([[04 Transactions]]) |
| Renvoie des **types du domaine** ou des structs « lignes d'écran » | Jamais de JSON |
| Ne lève pas pour un cas prévu | `Result<T>` |

### Un cas d'usage avec transaction

```cpp
// Prêter un livre : créer le prêt ET marquer le livre comme sorti, ensemble ou pas du tout.
domain::Result<domain::Id<domain::Loan>> LoanService::lend(NewLoan request)
{
    domain::Id<domain::Loan> created{};
    auto done = transactions_.run(
        [&]() -> domain::Result<void>
        {
            auto book = books_.get(request.bookId);
            if (!book)
            {
                return domain::fail(domain::errors::BookNotFound);
            }
            if (book->lent)
            {
                return domain::fail(domain::errors::BookAlreadyLent);
            }
            book->lent = true;
            (void)books_.update(*book);
            created = loans_.create({.bookId = request.bookId, .borrower = request.borrower,
                                     .lentOn = clock_.today()});
            return {};
        });
    if (!done)
    {
        return std::unexpected(done.error());
    }
    return created;
}
```

> [!note] Exemple d'extension, 2026-10-04
> `LoanService` n'existe pas dans le projet modèle : c'est l'exemple de la fonctionnalité suivante à ajouter. La marche à suivre complète est dans [[05 Ajouter une fonctionnalité]].

---

## 🔗 Liens

- [[02 Tester un service avec des faux]] — le tester sans base
- [[05 L'infrastructure]] — la suite
