---
tags:
  - projet/cpp
  - type/guide
  - techno/cpp
  - techno/doctest
  - sujet/architecture
  - statut/a-jour
aliases:
  - Tutoriel étape 4
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 4 — le domaine

> [!abstract] En une phrase
> On écrit les **données** et les **règles** : le résultat d'une opération (`Result`), les codes d'erreur, les identifiants typés, les dates, et le livre avec sa validation. Tout est testé **sans base ni fenêtre**. À lire avant : [[02 Le domaine]].

---

## 📄 1. Les briques communes

Dans `src/domain/common/`, dans cet ordre :

| Fichier | Contenu | Note |
|---|---|---|
| `result.hpp` | `Error`, `Result<T>`, `fail()` | [[01 Gérer les erreurs (expected et exceptions)]] |
| `errors.hpp` | Les codes stables | idem |
| `id.hpp` | `Id<T>` | [[02 Types forts]] |
| `dates.hpp` / `.cpp` | `Date`, `formatIso`, `parseIso` | [[03 Dates avec chrono]] |

## 📄 2. Le livre

`src/domain/book/book.hpp` :

```cpp
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
```

`src/domain/book/book.cpp` :

```cpp
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
```

Ajouter les `.cpp` dans `src/domain/CMakeLists.txt` :

```cmake
add_library(bookshelf_domain STATIC
    book/book.cpp
    common/dates.cpp
    common/text.cpp
)
```

## 🧪 3. Les tests

Copier `tests/domain/book_tests.cpp` et `tests/domain/dates_tests.cpp` ([[01 doctest — premiers tests]]), et les ajouter à `tests_domain`.

```powershell
cmake --build --preset debug
ctest --preset debug -R tests_domain
```

---

## ✅ Point d'étape

- [ ] `tests_domain` vert
- [ ] Le domaine n'inclut **que** la STL et lui-même (`layers` vert)
- [ ] Commit : « Domaine : livre et dates »

---

## 🔗 Liens

- [[03 Étape 3 — le frontend embarqué]] — l'étape d'avant
- [[05 Étape 5 — le service]] — la suite
