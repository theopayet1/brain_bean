---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - statut/a-jour
aliases:
  - std::ranges
  - Algorithmes STL
cree: 2026-10-04
maj: 2026-10-04
---

# Algorithmes et ranges

> [!abstract] En une phrase
> Plutôt qu'écrire une boucle pour trier, chercher ou compter, on appelle un **algorithme** de `<algorithm>`. La version **`std::ranges::`** (C++20) prend directement la collection et accepte une **projection** : « trie les livres **par titre** » s'écrit en une ligne.

---

## 🔃 Trier

```cpp
#include <algorithm>

std::ranges::sort(books, {}, &Book::title);                       // par titre
std::ranges::sort(books, std::ranges::greater{}, &Book::year);    // par année décroissante
std::ranges::sort(books, [](const Book& a, const Book& b)          // règle à la main
                  { return a.author < b.author || (a.author == b.author && a.title < b.title); });
```

| Argument | Sens |
|---|---|
| `books` | La collection |
| `{}` | Comparateur par défaut (`<`) |
| `&Book::title` | **Projection** : on compare `book.title`, pas le livre entier |

---

## 🔍 Chercher, compter, tester

```cpp
auto it = std::ranges::find(books, Id<Book>{42}, &Book::id);
if (it != books.end())
{
    std::println("{}", it->title);
}

auto unread = std::ranges::find_if(books, [](const Book& b) { return !b.read; });
auto count = std::ranges::count_if(books, &Book::read);         // nombre de livres lus
bool allRead = std::ranges::all_of(books, &Book::read);
bool anyOld = std::ranges::any_of(books, [](const Book& b) { return b.year < 1900; });
```

---

## 🧹 Filtrer, transformer

### Supprimer des éléments (C++20)

```cpp
std::erase_if(books, [](const Book& b) { return b.read; });   // retire les livres lus
```

### Une vue filtrée (sans copie)

```cpp
#include <ranges>

for (const Book& b : books | std::views::filter([](const Book& b) { return !b.read; }))
{
    std::println("À lire : {}", b.title);
}

auto titles = books | std::views::transform(&Book::title);       // vue sur les titres
std::vector<std::string> list(titles.begin(), titles.end());     // copie dans un vector
```

> [!warning] Une vue ne possède rien
> `books | std::views::filter(...)` **regarde** `books`. Si `books` disparaît, la vue aussi. Comme pour `string_view` : [[05 Durée de vie et pièges]].

---

## 📋 Les plus utiles

| Algorithme | Fait |
|---|---|
| `sort` / `stable_sort` | Trier (stable = garde l'ordre des égaux) |
| `find` / `find_if` | Premier élément égal / qui vérifie une condition |
| `count` / `count_if` | Compter |
| `all_of` / `any_of` / `none_of` | Tester |
| `min_element` / `max_element` | Plus petit / plus grand |
| `contains` (C++23) | `std::ranges::contains(ids, id)` |
| `std::erase_if(vec, f)` | Retirer ce qui vérifie `f` |
| `std::accumulate` (`<numeric>`) | Sommer |

---

## 🔗 Liens

- [[10 Lambdas]] — les conditions qu'on passe aux algorithmes
- [[05 Threads et file de tâches]] — la suite
