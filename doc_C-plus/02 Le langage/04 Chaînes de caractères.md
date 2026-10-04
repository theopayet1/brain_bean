---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - std::string
  - string_view
cree: 2026-10-04
maj: 2026-10-04
---

# Chaînes de caractères

> [!abstract] En une phrase
> **`std::string`** possède son texte (il le garde en mémoire), **`std::string_view`** regarde un texte qui appartient à quelqu'un d'autre sans le copier, et **`std::format`** fabrique un texte à partir de morceaux. En pratique : on stocke des `std::string`, on reçoit des `std::string_view`.

---

## 🧵 `std::string`

```cpp
#include <string>

std::string title = "Dune";
title += " Messiah";                 // concaténer
std::size_t n = title.size();        // nombre d'OCTETS (pas de lettres !)
bool empty = title.empty();
bool has = title.contains("Mess");   // C++23
bool starts = title.starts_with("Du");
std::string part = title.substr(0, 4);   // « Dune »
```

| Méthode | Fait |
|---|---|
| `size()` | Longueur en octets |
| `empty()` | Vide ? (préférer à `size() == 0`) |
| `find("x")` | Position, ou `std::string::npos` si absent |
| `substr(début, longueur)` | Un morceau |
| `starts_with` / `ends_with` / `contains` | Tests rapides |
| `c_str()` | Pointeur `const char*` terminé par `'\0'` (pour les API en C) |

---

## 👀 `std::string_view`

Une **vue** : un pointeur + une longueur. Elle ne copie rien.

```cpp
#include <string_view>

// Accepte une std::string, un "littéral", un morceau de texte : sans copie.
bool isIsbn(std::string_view text)
{
    return text.size() == 13;
}

isIsbn("9782266320481");
isIsbn(book.isbn);
```

> [!warning] Une vue ne garde rien en vie
> Si le texte regardé disparaît, la vue pointe dans le vide :
> ```cpp
> std::string_view danger()
> {
>     std::string local = "temporaire";
>     return local;   // ❌ local est détruit à la sortie : la vue est « pendante »
> }
> ```
> **Règle** : `string_view` en **paramètre**, oui. En **membre** de classe ou en **retour**, seulement si le texte vit forcément plus longtemps (un littéral, une constante). Voir [[05 Durée de vie et pièges]].

Les constantes de texte du programme sont des `string_view` : elles pointent dans l'exe, elles vivent toujours.

```cpp
inline constexpr std::string_view BookNotFound = "book.not_found";
```

---

## 🖨️ `std::format` et `std::println`

```cpp
#include <format>
#include <print>

std::string line = std::format("{} ({})", book.title, book.year.value_or(0));
std::println("Il y a {} livres", books.size());
std::println("{:>8}|{:<8}|", "droite", "gauche");   // alignements
std::println("{:.2f}", 3.14159);                     // 3.14
```

`{}` est remplacé par l'argument suivant. `{{` et `}}` écrivent une accolade.

---

## 🌍 UTF-8 et les accents

En UTF-8, `é` occupe **2 octets** : `std::string{"été"}.size()` vaut **5**.

- Compiler avec **`/utf-8`** (MSVC) : les littéraux restent en UTF-8.
- Enregistrer les fichiers sources en UTF-8.
- Ne jamais couper une chaîne au milieu d'un caractère accentué (`substr` sur des octets).

> [!info] Windows et l'UTF-16
> Les API Windows « larges » (`...W`) veulent du **UTF-16** (`std::wstring`, `wchar_t`). On convertit **aux bords**, dans l'[[05 L'infrastructure|infrastructure]], avec `MultiByteToWideChar` / `WideCharToMultiByte`. Tout le reste du programme reste en UTF-8.

---

## 🧽 Nettoyer un texte

```cpp
std::string trim(std::string_view text)
{
    constexpr std::string_view Spaces = " \t\r\n";
    const auto first = text.find_first_not_of(Spaces);
    if (first == std::string_view::npos)
    {
        return {};   // que des espaces
    }
    const auto last = text.find_last_not_of(Spaces);
    return std::string{text.substr(first, last - first + 1)};
}
```

| Code | Pourquoi |
|---|---|
| `find_first_not_of(Spaces)` | Position du premier caractère qui n'est pas un espace |
| `npos` | « pas trouvé » |
| `std::string{...}` | On renvoie une `string` qui **possède** son texte, pas une vue |

---

## 🔗 Liens

- [[05 Conteneurs de la STL]] — la suite
- [[05 Durée de vie et pièges]] — le piège des vues
