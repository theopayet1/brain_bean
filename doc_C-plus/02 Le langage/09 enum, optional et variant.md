---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - std::optional
  - std::variant
  - enum class
cree: 2026-10-04
maj: 2026-10-04
---

# `enum`, `optional` et `variant`

> [!abstract] En une phrase
> **`enum class`** : une valeur parmi une liste fixe (`Locked`, `Open`). **`std::optional<T>`** : un `T`, **ou rien**. **`std::variant<A, B>`** : un `A` **ou** un `B`. Trois façons de dire au compilateur exactement ce qui est possible, au lieu de codes magiques et de `nullptr`.

---

## 🔢 `enum class`

```cpp
enum class SortOrder
{
    Title,
    Author,
    AddedOn,
};

SortOrder order = SortOrder::Title;
```

> [!warning] Pas l'`enum` « nu »
> `enum Color { Red };` met `Red` dans l'espace de noms courant et se convertit en `int` sans prévenir. **Toujours `enum class`.**

Convertir vers / depuis du texte (pour le JSON ou la base) avec une table :

```cpp
std::optional<SortOrder> sortFrom(std::string_view text)
{
    static constexpr auto Orders = std::to_array<std::pair<std::string_view, SortOrder>>({
        {"title", SortOrder::Title},
        {"author", SortOrder::Author},
        {"added_on", SortOrder::AddedOn},
    });
    for (const auto& [name, order] : Orders)
    {
        if (name == text)
        {
            return order;
        }
    }
    return std::nullopt;   // texte inconnu
}
```

---

## ❓ `std::optional<T>`

```cpp
#include <optional>

std::optional<int> year;          // vide
year = 1965;                      // contient 1965
year = std::nullopt;              // de nouveau vide

if (year)                         // contient quelque chose ?
{
    std::println("{}", *year);    // * pour lire la valeur
}
int y = year.value_or(0);         // la valeur, ou 0 si vide
```

| Écriture | Sens |
|---|---|
| `if (opt)` / `opt.has_value()` | Est-ce qu'il y a une valeur ? |
| `*opt` / `opt->membre` | La valeur (⚠️ vide = comportement indéfini) |
| `opt.value()` | La valeur, ou exception si vide |
| `opt.value_or(défaut)` | La valeur, ou le défaut |
| `opt.transform(f)` | Applique `f` si plein, reste vide sinon |

Un usage typique : une fonction qui **cherche**.

```cpp
[[nodiscard]] virtual std::optional<Book> get(Id<Book> id) = 0;

auto book = books_.get(id);
if (!book)
{
    return fail(errors::BookNotFound);
}
book->read = true;
```

> [!tip] `optional` ou `expected` ?
> - « Pas trouvé » est **normal** et n'a pas besoin d'explication → `std::optional`.
> - L'échec a une **raison** à afficher (« titre vide », « année invalide ») → `std::expected`, voir [[01 Gérer les erreurs (expected et exceptions)]].

---

## 🔀 `std::variant<A, B, …>`

Une valeur qui est **l'un de plusieurs types**, et qui sait lequel :

```cpp
#include <variant>

using FieldValue = std::variant<std::string, std::int64_t, bool>;

FieldValue v = std::string{"Dune"};
v = true;

if (std::holds_alternative<bool>(v))
{
    bool b = std::get<bool>(v);
}
```

Traiter **tous** les cas avec `std::visit` (le compilateur refuse si un cas manque) :

```cpp
template <typename... F>
struct Overloaded : F...
{
    using F::operator()...;
};

std::string describe(const FieldValue& value)
{
    return std::visit(Overloaded{
                          [](const std::string& s) { return s; },
                          [](std::int64_t n) { return std::to_string(n); },
                          [](bool b) { return std::string{b ? "oui" : "non"}; },
                      },
                      value);
}
```

> [!example] Où ça sert
> Un formulaire dont les champs sont configurables : chaque réponse est un texte, un nombre ou une case cochée. Une `std::map<std::string, FieldValue>` les range **sans JSON** dans le domaine.

---

## 🔗 Liens

- [[01 Gérer les erreurs (expected et exceptions)]] — `std::expected`, le cousin d'`optional`
- [[10 Lambdas]] — la suite
