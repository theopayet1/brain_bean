---
tags:
  - projet/cpp
  - type/guide
  - techno/doctest
  - techno/sqlite
  - sujet/tests
  - sujet/base-de-donnees
  - statut/a-jour
aliases:
  - Tester SQLite
cree: 2026-10-04
maj: 2026-10-04
---

# Tester la base de données

> [!abstract] En une phrase
> Les dépôts SQLite se testent sur une **vraie base**, dans un **fichier temporaire** unique par test, supprimé à la fin. On vérifie l'aller-retour (écrire puis relire à l'identique), la recherche, les migrations rejouées, et l'annulation des transactions.

---

## 📄 Le fichier temporaire : `tests/support/temp_database.hpp`

```cpp
#pragma once

#include <filesystem>
#include <random>
#include <string>

namespace bookshelf::tests
{

// Un fichier de base unique dans le dossier temporaire, supprimé à la fin du test.
class TempDatabase
{
public:
    TempDatabase()
        : path_(std::filesystem::temp_directory_path() /
                ("bookshelf-test-" + std::to_string(std::random_device{}()) + ".db"))
    {
    }

    ~TempDatabase()
    {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }

    TempDatabase(const TempDatabase&) = delete;
    TempDatabase& operator=(const TempDatabase&) = delete;

    [[nodiscard]] const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace bookshelf::tests
```

| Point | Pourquoi |
|---|---|
| Nom aléatoire | Deux tests en parallèle n'utilisent pas le même fichier |
| Suppression dans le destructeur | RAII : même si le test échoue, le fichier disparaît |
| `std::error_code` | `remove` ne lève pas dans un destructeur |

> [!tip] Et `:memory:` ?
> SQLite accepte le chemin spécial `":memory:"` : une base en RAM, encore plus rapide. Le fichier temporaire teste en plus le **vrai** comportement disque (migrations, WAL, verrous).

---

## 📄 Les tests : `tests/infrastructure/books_sqlite_tests.cpp`

```cpp
#include "infrastructure/sqlite/books_sqlite.hpp"
#include "infrastructure/sqlite/migrations.hpp"
#include "support/temp_database.hpp"

#include <doctest/doctest.h>

using namespace bookshelf;
using namespace bookshelf::infrastructure::sqlite;

TEST_CASE("un livre enregistré se relit à l'identique")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    BooksSqlite books{connection};

    const auto id = books.create({
        .title = "Dune",
        .author = "Frank Herbert",
        .year = 1965,
        .addedOn = std::chrono::year{2026} / 10 / 4,
    });
    const auto read = books.get(id);

    REQUIRE(read.has_value());
    CHECK(read->title == "Dune");
    CHECK(read->year == 1965);
    CHECK(read->addedOn == std::chrono::year{2026} / 10 / 4);
}

TEST_CASE("la recherche trouve par titre ou par auteur")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    BooksSqlite books{connection};
    (void)books.create({.title = "Dune", .author = "Frank Herbert"});
    (void)books.create({.title = "Fondation", .author = "Isaac Asimov"});

    CHECK(books.list("").size() == 2);
    CHECK(books.list("Asimov").size() == 1);
    CHECK(books.list("Dun").front().title == "Dune");
}

TEST_CASE("migrer deux fois ne casse rien")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    CHECK_NOTHROW(migrate(connection));
}

TEST_CASE("une transaction non validée est annulée")
{
    const tests::TempDatabase file;
    Connection connection{file.path()};
    migrate(connection);
    BooksSqlite books{connection};
    {
        Transaction transaction{connection};
        (void)books.create({.title = "Dune"});
        // pas de commit() : le destructeur annule
    }
    CHECK(books.list("").empty());
}
```

| Test | Ce qu'il garantit |
|---|---|
| Aller-retour | `readBook` lit les colonnes dans le bon ordre, les dates et les `NULL` survivent |
| Recherche | Le SQL de `list` est juste |
| Migrer deux fois | `migrate()` est **idempotent** : relancer l'appli ne casse rien |
| Transaction non validée | Le destructeur annule bien |

`(void)books.create(...)` : on ignore volontairement le résultat d'une fonction `[[nodiscard]]`.

---

## 🔗 Liens

- [[03 Un dépôt SQLite]] — le code testé
- [[04 Tester le pont]] — la suite
