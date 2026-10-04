---
tags:
  - projet/cpp
  - type/guide
  - techno/sqlite
  - techno/cpp
  - sujet/base-de-donnees
  - sujet/architecture
  - statut/a-jour
aliases:
  - Repository SQLite
  - BooksSqlite
cree: 2026-10-04
maj: 2026-10-04
---

# Un dépôt SQLite

> [!abstract] En une phrase
> Un **dépôt** (*repository*) est la classe de l'infrastructure qui **implémente un port** avec du SQL : `BooksSqlite` implémente `IBooks`. Chaque méthode prépare une requête, **lie** les paramètres, et transforme les lignes en structs du domaine. Le passage ligne → struct est écrit à la main, dans une seule fonction.

---

## 📄 `books_sqlite.hpp`

```cpp
#pragma once

#include "application/ports/books.hpp"
#include "infrastructure/sqlite/connection.hpp"

namespace bookshelf::infrastructure::sqlite
{

// Le port IBooks, rangé dans SQLite.
class BooksSqlite : public application::IBooks
{
public:
    explicit BooksSqlite(Connection& connection);

    domain::Id<domain::Book> create(const domain::Book& book) override;
    std::optional<domain::Book> get(domain::Id<domain::Book> id) override;
    bool update(const domain::Book& book) override;
    bool remove(domain::Id<domain::Book> id) override;
    std::vector<domain::Book> list(std::string_view search) override;

private:
    Connection& connection_;
};

} // namespace bookshelf::infrastructure::sqlite
```

## 📄 `books_sqlite.cpp`

```cpp
#include "infrastructure/sqlite/books_sqlite.hpp"

#include "domain/common/dates.hpp"

#include <format>

namespace bookshelf::infrastructure::sqlite
{

namespace
{

// Toujours les mêmes colonnes, dans le même ordre : readBook() lit par position.
constexpr std::string_view Columns = "id, title, author, year, read, added_on";

domain::Book readBook(const Statement& row)
{
    return {
        .id = {row.integer(0)},
        .title = row.text(1),
        .author = row.text(2),
        .year = row.integerOrNull(3).transform([](std::int64_t y) { return static_cast<int>(y); }),
        .read = row.integer(4) != 0,
        .addedOn = domain::parseIso(row.text(5)).value_or(domain::Date{}),
    };
}

} // namespace

BooksSqlite::BooksSqlite(Connection& connection)
    : connection_(connection)
{
}

domain::Id<domain::Book> BooksSqlite::create(const domain::Book& book)
{
    connection_
        .prepare("INSERT INTO book (title, author, year, read, added_on) VALUES (?, ?, ?, ?, ?)")
        .bind(book.title)
        .bind(book.author)
        .bind(book.year)
        .bind(book.read)
        .bind(domain::formatIso(book.addedOn))
        .run();
    return {connection_.lastInsertId()};
}

std::optional<domain::Book> BooksSqlite::get(domain::Id<domain::Book> id)
{
    auto statement = connection_.prepare(std::format("SELECT {} FROM book WHERE id = ?", Columns));
    statement.bind(id.value);
    if (!statement.next())
    {
        return std::nullopt;
    }
    return readBook(statement);
}

bool BooksSqlite::update(const domain::Book& book)
{
    connection_.prepare("UPDATE book SET title = ?, author = ?, year = ?, read = ? WHERE id = ?")
        .bind(book.title)
        .bind(book.author)
        .bind(book.year)
        .bind(book.read)
        .bind(book.id.value)
        .run();
    return connection_.changes() > 0;
}

bool BooksSqlite::remove(domain::Id<domain::Book> id)
{
    connection_.prepare("DELETE FROM book WHERE id = ?").bind(id.value).run();
    return connection_.changes() > 0;
}

std::vector<domain::Book> BooksSqlite::list(std::string_view search)
{
    // Le texte cherché est un PARAMÈTRE, jamais collé dans le SQL : pas d'injection SQL.
    auto statement = connection_.prepare(
        std::format("SELECT {} FROM book WHERE ?1 = '' OR title LIKE '%' || ?1 || '%' "
                    "OR author LIKE '%' || ?1 || '%' ORDER BY title COLLATE NOCASE",
                    Columns));
    statement.bind(search);

    std::vector<domain::Book> books;
    while (statement.next())
    {
        books.push_back(readBook(statement));
    }
    return books;
}

} // namespace bookshelf::infrastructure::sqlite
```

---

## 🔍 Les points importants

| Point | Pourquoi |
|---|---|
| `Columns` + `readBook()` | **Un seul endroit** sait dans quel ordre sont les colonnes. Toutes les requêtes `SELECT {Columns}` |
| `readBook` construit le `Book` avec des **initialisations désignées** | On voit quelle colonne remplit quel champ |
| `integerOrNull(3).transform(...)` | `NULL` → `std::nullopt`, sinon conversion en `int` |
| Dates en texte ISO (`formatIso` / `parseIso`) | Lisible, triable ([[03 Dates avec chrono]]) |
| `changes() > 0` | `UPDATE` / `DELETE` sur un id inexistant ne modifie aucune ligne → `false` |
| `?1` réutilisé trois fois | Paramètre **numéroté** : lié une fois, utilisé plusieurs fois |
| `LIKE '%' \|\| ?1 \|\| '%'` | La recherche « contient », sans jamais coller le texte dans le SQL |
| `ORDER BY title COLLATE NOCASE` | Tri sans tenir compte des majuscules |

---

## 📊 Une requête « lecture d'écran »

Quand un écran a besoin de données **calculées** (un nombre de prêts par livre, la date du dernier prêt), on n'empile pas les appels : une requête dédiée renvoie directement les lignes de l'écran.

```cpp
// application/books/rows.hpp : la forme d'une ligne de l'écran « Mes livres »
struct BookRow
{
    domain::Id<domain::Book> id;
    std::string title;
    std::int64_t loanCount = 0;
    std::optional<domain::Date> lastLoan;
};

// application/ports/book_reads.hpp
class IBookReads
{
public:
    virtual ~IBookReads() = default;
    [[nodiscard]] virtual std::vector<BookRow> rows(std::string_view search) = 0;
};
```

```sql
SELECT b.id, b.title, count(l.id), max(l.lent_on)
FROM book b
LEFT JOIN loan l ON l.book_id = b.id
WHERE ?1 = '' OR b.title LIKE '%' || ?1 || '%'
GROUP BY b.id
ORDER BY b.title COLLATE NOCASE
```

> [!tip] Lectures séparées des écritures
> Les dépôts gardent les **écritures** et le **chargement par id**. Les **listes d'écran** ont leur port de lecture. Chaque écran a sa requête optimale, sans tordre les dépôts.

---

## 🔗 Liens

- [[03 Les ports (interfaces)]] — le contrat implémenté ici
- [[03 Tester la base de données]] — les tests de ce dépôt
- [[04 Transactions]] — la suite
