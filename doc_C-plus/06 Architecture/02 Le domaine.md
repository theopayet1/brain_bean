---
tags:
  - projet/cpp
  - type/archi
  - techno/cpp
  - sujet/architecture
  - statut/a-jour
aliases:
  - Domain C++
  - Couche domaine
cree: 2026-10-04
maj: 2026-10-04
---

# Le domaine

> [!abstract] En une phrase
> Le **domaine** contient les **données** de l'application (des structs simples) et les **règles** qui sont vraies partout (« un livre a un titre », « une année est entre 1400 et 2100 »). Il n'utilise **que la STL** : pas de base, pas de JSON, pas de fenêtre. C'est la partie la plus facile à tester et la plus stable.

---

## 📂 Ce qu'il contient

```text
src/domain/
├─ CMakeLists.txt
├─ common/
│  ├─ result.hpp     Result<T>, Error, fail()
│  ├─ errors.hpp     les codes d'erreur stables
│  ├─ id.hpp         Id<T>
│  ├─ dates.hpp/.cpp Date, formatIso, parseIso
│  └─ text.hpp/.cpp  trim
└─ book/
   └─ book.hpp/.cpp  Book, validate()
```

Un sous-dossier par **notion métier** (`book/`, `loan/`, `member/`…) et un `common/` pour ce qui sert partout.

---

## 📦 Une entité : `Book`

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

| Choix | Pourquoi |
|---|---|
| Une **struct** aux membres publics | C'est une donnée ; les règles sont dans `validate()` |
| `Id<Book>` | Type fort ([[02 Types forts]]) |
| `std::optional<int> year` | « Année inconnue » est une vraie possibilité |
| `Date addedOn` | `std::chrono`, pas de texte : on calcule avec |
| `validate()` prend le livre **par valeur** et le renvoie nettoyé | Une seule fonction vérifie **et** normalise ; l'appelant récupère la version propre |
| `Result<Book>` | Échec prévu avec un code ([[01 Gérer les erreurs (expected et exceptions)]]) |

---

## 🧰 Les briques communes

`result.hpp` et `errors.hpp` : voir [[01 Gérer les erreurs (expected et exceptions)]]. `id.hpp` : [[02 Types forts]]. `dates.hpp` : [[03 Dates avec chrono]].

```cpp
#pragma once

#include <string>
#include <string_view>

namespace bookshelf::domain
{

// Retire les espaces au début et à la fin.
[[nodiscard]] std::string trim(std::string_view text);

} // namespace bookshelf::domain
```

---

## ✅ Ce qui va dans le domaine, ou pas

| Oui | Non |
|---|---|
| `struct Book`, `struct Loan` | `BooksSqlite` (infrastructure) |
| `validate(Book)`, `isOverdue(Loan, Date today)` | « lire la date du jour » (c'est une horloge : un port) |
| Les codes d'erreur | Les **messages** en français (pont) |
| Calculs : un total, un âge, une durée | Formatage pour l'affichage (frontend) |
| `std::map<std::string, std::variant<...>>` pour des valeurs libres | Du JSON (`glz::json_t`) |

> [!tip] Une règle pure = une fonction qui reçoit tout ce qu'il lui faut
> `isOverdue(loan, today)` reçoit la date du jour **en paramètre** au lieu de la lire elle-même. Elle se teste alors avec n'importe quelle date, sans horloge.

---

## 🔗 Liens

- [[01 doctest — premiers tests]] — tester le domaine
- [[03 Les ports (interfaces)]] — la suite
