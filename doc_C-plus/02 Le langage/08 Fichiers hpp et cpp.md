---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - sujet/compilation
  - statut/a-jour
aliases:
  - Headers C++
  - En-têtes C++
cree: 2026-10-04
maj: 2026-10-04
---

# Fichiers `.hpp` et `.cpp`

> [!abstract] En une phrase
> Chaque morceau de code est coupé en deux : le **`.hpp` (en-tête)** dit **ce qui existe** (les déclarations, le « sommaire »), le **`.cpp` (source)** dit **comment ça marche** (les définitions). Les autres fichiers n'incluent que le `.hpp`. À lire avant : [[04 De la source à l'exe]].

---

## 🧩 Le couple

`src/domain/book/book.hpp` :

```cpp
#pragma once                                   // 👈 n'inclure qu'une fois

#include "domain/common/dates.hpp"
#include "domain/common/id.hpp"
#include "domain/common/result.hpp"

#include <optional>
#include <string>

namespace bookshelf::domain
{

struct Book
{
    Id<Book> id;
    std::string title;
    std::string author;
    std::optional<int> year;
    bool read = false;
    Date addedOn{};
};

inline constexpr std::size_t MaxTitleLength = 200;

[[nodiscard]] Result<Book> validate(Book book);   // 👈 déclaration seule

} // namespace bookshelf::domain
```

`src/domain/book/book.cpp` :

```cpp
#include "domain/book/book.hpp"      // 👈 toujours SON en-tête en premier

#include "domain/common/errors.hpp"
#include "domain/common/text.hpp"

namespace bookshelf::domain
{

Result<Book> validate(Book book)      // 👈 définition
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
    if (book.year && (*book.year < 1400 || *book.year > 2100))
    {
        return fail(errors::BookYearInvalid);
    }
    return book;
}

} // namespace bookshelf::domain
```

---

## 📌 Ce qui va où

| Dans le `.hpp` | Dans le `.cpp` |
|---|---|
| `struct` / `class` (leur forme) | Le corps des fonctions et méthodes |
| Déclarations de fonctions `...;` | Les fonctions d'aide internes (dans un `namespace { }` anonyme) |
| `inline constexpr` constantes | Les `#include` dont seul le `.cpp` a besoin |
| Les **templates** (en entier, voir [[11 Templates]]) | — |
| Petites méthodes d'une ligne (`int code() const { return code_; }`) | — |

> [!tip] Inclure le moins possible dans un `.hpp`
> Tout ce qu'un `.hpp` inclut est recompilé par **chaque** fichier qui l'inclut. Si un `.hpp` a juste besoin de savoir qu'un type existe (pointeur ou référence), une **déclaration anticipée** suffit :
> ```cpp
> struct sqlite3;          // 👈 « ce type existe », sans inclure <sqlite3.h>
> class Connection { std::unique_ptr<sqlite3, Close> db_; };
> ```

---

## 🛡️ `#pragma once`

Sans lui, un `.hpp` inclus deux fois (directement et via un autre en-tête) donne « `Book` : redéfinition ». **Première ligne de chaque `.hpp`**.

---

## 🏠 Les `namespace`

Un **espace de noms** range les noms pour éviter les collisions (`bookshelf::domain::Book` ne se confond pas avec un `Book` d'une bibliothèque).

```cpp
namespace bookshelf::domain
{
// ...
} // namespace bookshelf::domain
```

| Règle | Exemple |
|---|---|
| **namespace = dossier** | `src/domain/book/` → `bookshelf::domain` |
| Un `namespace { }` **anonyme** dans un `.cpp` | Ce qui est dedans est **privé au fichier** |
| `using namespace` | Jamais dans un `.hpp`. Dans un `.cpp` ou un test, d'accord |

```cpp
namespace
{

// Visible seulement dans ce .cpp : pas de conflit possible avec un autre fichier.
bool readNumber(std::string_view text, int& value) { /* ... */ }

} // namespace
```

---

## 📂 Les chemins d'inclusion

Une seule **racine** : `src/`. Tous les `#include` du projet partent de là, ce qui dit tout de suite **de quelle couche** vient un fichier :

```cpp
#include "domain/book/book.hpp"
#include "application/ports/books.hpp"
```

- `"…"` pour les fichiers du projet, `<…>` pour la bibliothèque standard et les bibliothèques externes.
- CMake déclare la racine : `target_include_directories(... PUBLIC src)`, voir [[02 CMake — une bibliothèque par couche]].
- clang-format range les `#include` par groupes tout seul ([[06 clang-format et clang-tidy]]).

---

## ⚠️ Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| `redefinition of 'struct Book'` | `#pragma once` oublié | L'ajouter |
| `LNK2005 ... already defined` | Une fonction ou variable **définie** dans un `.hpp` | `inline` devant, ou déplacer la définition dans le `.cpp` |
| `LNK2019 unresolved external` | Déclarée dans le `.hpp`, jamais définie, ou `.cpp` absent de CMake | Écrire le corps, ajouter le `.cpp` dans `add_library` |
| Inclusions circulaires (`a.hpp` inclut `b.hpp` qui inclut `a.hpp`) | Deux types qui se connaissent | Déclaration anticipée, ou revoir le découpage |

---

## 🔗 Liens

- [[04 De la source à l'exe]] — pourquoi ce découpage
- [[02 CMake — une bibliothèque par couche]] — la racine d'inclusion
- [[09 enum, optional et variant]] — la suite
