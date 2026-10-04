---
tags:
  - projet/cpp
  - type/guide
  - techno/doctest
  - sujet/tests
  - sujet/injection-de-dependances
  - statut/a-jour
aliases:
  - Fakes C++
  - Faux dépôts
cree: 2026-10-04
maj: 2026-10-04
---

# Tester un service avec des faux

> [!abstract] En une phrase
> Un service reçoit ses ports par son constructeur ([[04 Les services (cas d'usage)]]). Dans les tests, on lui donne des **faux** : `FakeBooks` range les livres dans une `std::map`, `FixedClock` renvoie toujours la même date. Le test est alors **instantané**, **déterministe** et ne touche à aucun fichier.

---

## 📄 Les faux : `tests/support/fakes.hpp`

```cpp
#pragma once

#include "application/ports/books.hpp"
#include "application/ports/clock.hpp"

#include <algorithm>
#include <map>

namespace bookshelf::tests
{

// Une date fixe : le test donne le même résultat aujourd'hui et dans dix ans.
class FixedClock : public application::IClock
{
public:
    explicit FixedClock(std::chrono::year_month_day day = std::chrono::year{2026} / 10 / 4)
        : day_(day)
    {
    }

    [[nodiscard]] std::chrono::year_month_day today() const override { return day_; }

private:
    std::chrono::year_month_day day_;
};

// Des livres rangés dans une std::map, en mémoire. Aucune base, aucun fichier.
class FakeBooks : public application::IBooks
{
public:
    domain::Id<domain::Book> create(const domain::Book& book) override
    {
        domain::Book copy = book;
        copy.id = {nextId_++};
        books_[copy.id.value] = copy;
        return copy.id;
    }

    std::optional<domain::Book> get(domain::Id<domain::Book> id) override
    {
        const auto found = books_.find(id.value);
        if (found == books_.end())
        {
            return std::nullopt;
        }
        return found->second;
    }

    bool update(const domain::Book& book) override
    {
        const auto found = books_.find(book.id.value);
        if (found == books_.end())
        {
            return false;
        }
        found->second = book;
        return true;
    }

    bool remove(domain::Id<domain::Book> id) override { return books_.erase(id.value) > 0; }

    std::vector<domain::Book> list(std::string_view search) override
    {
        std::vector<domain::Book> result;
        for (const auto& [id, book] : books_)
        {
            if (search.empty() || book.title.contains(search) || book.author.contains(search))
            {
                result.push_back(book);
            }
        }
        std::ranges::sort(result, {}, &domain::Book::title);
        return result;
    }

private:
    std::map<std::int64_t, domain::Book> books_;
    std::int64_t nextId_ = 1;
};

} // namespace bookshelf::tests
```

| Point | Pourquoi |
|---|---|
| `FakeBooks` respecte le **même contrat** que `BooksSqlite` | id attribué, `false` si absent, tri par titre : sinon le test prouve des choses fausses |
| `FixedClock` avec une date par défaut | Le test qui n'en a pas besoin n'a rien à préciser |
| Tout dans un `.hpp` | Partagé par tous les tests de l'application |

> [!info] Faux, bouchon, espion
> - **Faux** (*fake*) : une vraie implémentation simplifiée (`std::map` au lieu de SQLite). C'est ce qu'on utilise ici.
> - **Bouchon** (*stub*) : renvoie des réponses fixées d'avance.
> - **Espion** (*spy*) : retient comment il a été appelé, pour le vérifier.
>
> En C++, pas besoin de bibliothèque de *mocks* : une petite classe qui hérite du port suffit.

---

## 📄 Les tests : `tests/application/book_service_tests.cpp`

```cpp
#include "application/books/book_service.hpp"
#include "domain/common/errors.hpp"
#include "support/fakes.hpp"

#include <doctest/doctest.h>

using namespace bookshelf;
using bookshelf::tests::FakeBooks;
using bookshelf::tests::FixedClock;

TEST_CASE("ajouter un livre lui donne la date du jour")
{
    FakeBooks books;
    const FixedClock clock{std::chrono::year{2026} / 10 / 4};
    application::BookService service{books, clock};

    const auto id = service.add({.title = "Dune", .author = "Frank Herbert", .year = 1965});

    REQUIRE(id.has_value());
    const auto saved = books.get(*id);
    REQUIRE(saved.has_value());
    CHECK(saved->addedOn == std::chrono::year{2026} / 10 / 4);
    CHECK_FALSE(saved->read);
}

TEST_CASE("un livre invalide n'est jamais enregistré")
{
    FakeBooks books;
    const FixedClock clock;
    application::BookService service{books, clock};

    const auto id = service.add({.title = ""});

    CHECK(id.error().code == domain::errors::BookTitleMissing);
    CHECK(books.list("").empty());
}

TEST_CASE("marquer comme lu un livre qui n'existe pas renvoie une erreur")
{
    FakeBooks books;
    const FixedClock clock;
    application::BookService service{books, clock};

    const auto result = service.markAsRead({42}, true);

    CHECK(result.error().code == domain::errors::BookNotFound);
}
```

| Ce qu'on vérifie | Pourquoi |
|---|---|
| La date d'ajout vient de **l'horloge** | Le service utilise bien le port, pas `system_clock` |
| Un livre invalide n'est **jamais** enregistré | L'ordre « valider puis créer » est respecté |
| Le bon **code** d'erreur pour un livre inexistant | Le contrat avec le pont |

---

## 🕵️ Un espion, quand il faut vérifier un appel

```cpp
class SpyLog
{
public:
    void error(std::string_view where, std::string_view detail)
    {
        lines.push_back(std::string{where} + ": " + std::string{detail});
    }
    std::vector<std::string> lines;
};

// … puis : CHECK(spy.lines.size() == 1);
```

---

## 🔗 Liens

- [[03 Les ports (interfaces)]] — ce que les faux implémentent
- [[03 Tester la base de données]] — la suite
