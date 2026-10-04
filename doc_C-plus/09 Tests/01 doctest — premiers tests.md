---
tags:
  - projet/cpp
  - type/guide
  - techno/doctest
  - sujet/tests
  - statut/a-jour
aliases:
  - doctest
cree: 2026-10-04
maj: 2026-10-04
---

# doctest — premiers tests

> [!abstract] En une phrase
> **doctest** est une bibliothèque de tests en **un seul en-tête** : on écrit `TEST_CASE("ce qu'on vérifie")`, on y met des `CHECK(condition)`, et il fournit le `main()` qui lance tout et affiche les échecs. Les noms de tests sont des **phrases** en français.

---

## 📄 Le `main` des tests

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
```

`DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN` : doctest écrit le `main()`. Ce fichier est le même pour tous les exe de tests.

---

## 📄 Tester le domaine : `tests/domain/book_tests.cpp`

```cpp
#include "domain/book/book.hpp"
#include "domain/common/errors.hpp"

#include <doctest/doctest.h>

using namespace bookshelf::domain;

TEST_CASE("un livre valide est accepté et nettoyé")
{
    const auto book = validate({.title = "  Dune  ", .author = "Frank Herbert", .year = 1965});

    REQUIRE(book.has_value());
    CHECK(book->title == "Dune");
}

TEST_CASE("un livre sans titre est refusé")
{
    const auto book = validate({.title = "   ", .author = "Anonyme"});

    REQUIRE_FALSE(book.has_value());
    CHECK(book.error().code == errors::BookTitleMissing);
}

TEST_CASE("une année absurde est refusée")
{
    CHECK(validate({.title = "Dune", .year = 12}).error().code == errors::BookYearInvalid);
    CHECK(validate({.title = "Dune", .year = 1965}).has_value());
    CHECK(validate({.title = "Dune", .year = std::nullopt}).has_value());
}
```

## 📄 `tests/domain/dates_tests.cpp`

```cpp
#include "domain/common/dates.hpp"

#include <doctest/doctest.h>

using namespace bookshelf::domain;
using namespace std::chrono;

TEST_CASE("une date fait l'aller-retour en texte ISO")
{
    const Date date = year{2026} / 10 / 4;
    CHECK(formatIso(date) == "2026-10-04");
    CHECK(parseIso("2026-10-04") == date);
}

TEST_CASE("les fausses dates sont refusées")
{
    CHECK_FALSE(parseIso("2026-02-30").has_value());
    CHECK_FALSE(parseIso("04/10/2026").has_value());
    CHECK_FALSE(parseIso("").has_value());
    CHECK_FALSE(parseIso("2026-1a-04").has_value());
}
```

---

## 🧰 Les macros

| Macro | Si faux |
|---|---|
| `CHECK(x)` | Note l'échec et **continue** le test |
| `REQUIRE(x)` | Note l'échec et **arrête** le test |
| `CHECK_FALSE(x)` / `REQUIRE_FALSE(x)` | Pareil, pour « doit être faux » |
| `CHECK_THROWS(expr)` / `CHECK_NOTHROW(expr)` | Doit lever / ne doit pas lever |
| `CHECK_THROWS_AS(expr, Type)` | Doit lever une exception de ce type |
| `SUBCASE("...")` | Un sous-scénario : le `TEST_CASE` est rejoué depuis le début pour chaque `SUBCASE` |

> [!tip] `REQUIRE` avant de déréférencer
> ```cpp
> REQUIRE(book.has_value());     // si c'est faux, on s'arrête là…
> CHECK(book->title == "Dune");  // …sinon cette ligne lirait un optional vide (UB)
> ```

---

## ✍️ Bien écrire un test

| Règle | Exemple |
|---|---|
| Le nom est une **phrase** qui dit ce qui doit être vrai | « un livre sans titre est refusé » |
| Trois temps : **préparer, agir, vérifier** (lignes vides entre les trois) | voir `book_service_tests.cpp` |
| Un comportement par test | Plutôt 3 petits tests qu'un gros |
| On vérifie le **code** d'erreur, pas le message | `error().code == errors::BookTitleMissing` |
| Aucune dépendance au jour, à l'heure, au hasard | Horloge fixe ([[02 Tester un service avec des faux]]) |

---

## 🔗 Liens

- [[02 Le domaine]] — ce qui est testé ici
- [[02 Tester un service avec des faux]] — la suite
