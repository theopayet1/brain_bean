---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - statut/a-jour
aliases:
  - vector map C++
  - Conteneurs C++
cree: 2026-10-04
maj: 2026-10-04
---

# Conteneurs de la STL

> [!abstract] En une phrase
> Un **conteneur** range plusieurs valeurs. Retiens-en trois : **`std::vector`** (une liste, le choix par défaut), **`std::map`** (des valeurs rangées par clé, triées) et **`std::array`** (une liste de taille fixe connue à la compilation).

---

## 📋 `std::vector` : le choix par défaut

```cpp
#include <vector>

std::vector<Book> books;
books.push_back(book);                 // ajouter à la fin
books.emplace_back(Book{.title = "Dune"});
std::size_t n = books.size();
const Book& first = books.front();     // ⚠️ liste vide = comportement indéfini
const Book& third = books[2];          // ⚠️ pas de vérification
const Book& safe = books.at(2);        // lève une exception si hors limites
books.clear();
books.reserve(100);                    // prévoit la place (évite des réallocations)
```

> [!warning] Les références sur un vector qui grandit
> ```cpp
> Book& b = books.front();
> books.push_back(other);   // le vector peut DÉMÉNAGER en mémoire
> b.title = "x";            // ❌ b pointe peut-être dans l'ancien emplacement
> ```

---

## 🗂️ Le bon conteneur

| Conteneur | C'est | Quand |
|---|---|---|
| `std::vector<T>` | Liste contiguë | **Par défaut**, presque toujours |
| `std::array<T, N>` | Liste de taille fixe | Tables constantes : `std::to_array({...})` |
| `std::map<K, V>` | Clé → valeur, **triée** par clé | Recherche par clé, parcours dans l'ordre |
| `std::unordered_map<K, V>` | Clé → valeur, non triée, plus rapide | Grosse table de recherche |
| `std::set<T>` | Valeurs uniques triées | « Est-ce que j'ai déjà vu X ? » |
| `std::deque<T>` | Ajout / retrait rapide aux **deux bouts** | File d'attente de tâches ([[05 Threads et file de tâches]]) |

---

## 🗺️ `std::map`

```cpp
#include <map>

std::map<std::int64_t, Book> byId;
byId[book.id.value] = book;                  // insère ou remplace

if (auto found = byId.find(42); found != byId.end())
{
    std::println("{}", found->second.title);  // first = clé, second = valeur
}

bool removed = byId.erase(42) > 0;

for (const auto& [id, b] : byId) { /* dans l'ordre des clés */ }
```

> [!warning] `map[clé]` crée la clé si elle manque
> Pour **lire**, utiliser `find` ou `contains`, jamais `[]` : sinon une recherche ajoute une entrée vide.

---

## 🧱 `std::array` et les tables constantes

Une table connue à la compilation, sans allocation :

```cpp
#include <array>

static constexpr auto Messages = std::to_array<std::pair<std::string_view, std::string_view>>({
    {"book.not_found", "Ce livre n'existe plus."},
    {"book.title_missing", "Le titre est obligatoire."},
});

for (const auto& [code, message] : Messages) { /* ... */ }
```

Pour quelques dizaines d'éléments, parcourir une `array` est plus simple et aussi rapide qu'une `map`.

---

## 📐 Comparatif rapide

| Opération | `vector` | `map` | `unordered_map` |
|---|---|---|---|
| Ajouter | à la fin : rapide | moyen | rapide |
| Chercher par valeur / clé | lent (tout parcourir) | rapide | très rapide |
| Ordre | celui d'insertion | trié | aucun |

---

## 🔗 Liens

- [[04 Algorithmes et ranges]] — trier, filtrer, chercher dans un vector
- [[06 Références et pointeurs]] — la suite
