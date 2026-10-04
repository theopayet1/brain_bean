---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - Template C++
  - Généricité C++
cree: 2026-10-04
maj: 2026-10-04
---

# Templates

> [!abstract] En une phrase
> Un **template** est un « moule » : on écrit une fonction ou une classe **une fois** avec un type à trou (`T`), et le compilateur fabrique une version pour chaque type utilisé. `std::vector<T>`, `std::optional<T>` sont des templates ; tu en écriras quelques petits, pas des montagnes.

---

## 🍪 Une fonction template

```cpp
template <typename T>
T biggest(T a, T b)
{
    return a > b ? a : b;
}

biggest(3, 7);          // T = int
biggest(2.5, 1.0);      // T = double
```

---

## 🧱 Une classe template : le type fort

```cpp
template <typename T>
struct Id
{
    std::int64_t value = 0;

    friend constexpr auto operator<=>(Id, Id) = default;
};

Id<Book> bookId{1};
Id<Loan> loanId{1};
bookId == loanId;   // ❌ erreur de compilation : deux types différents
```

`T` n'est même pas utilisé dans la struct : il sert juste à **fabriquer des types différents**. Voir [[02 Types forts]].

---

## 🏷️ Les alias de type

```cpp
template <typename T>
using Result = std::expected<T, Error>;

Result<Book> validate(Book book);   // = std::expected<Book, Error>
```

`using Nom = Type;` donne un nom court à un type long, avec ou sans template.

---

## 📜 Un template de lecture JSON

Un vrai exemple : lire n'importe quel type de demande de la même façon.

```cpp
template <typename T>
domain::Result<T> read(std::string_view json)
{
    T value{};
    if (glz::read<Strict>(value, json))
    {
        return fail(errors::BridgeInvalidRequest);
    }
    return value;
}

auto request = read<dto::AddBookRequest>(json);   // 👈 on précise T entre < >
```

---

## 🧷 Les concepts (C++20) : dire ce que `T` doit savoir faire

```cpp
#include <concepts>

template <std::integral T>          // 👈 T doit être un type entier
T half(T value)
{
    return value / 2;
}

half(10);     // ✅
half(2.5);    // ❌ message clair : double n'est pas integral
```

---

## 📌 Règles pratiques

| Règle | Pourquoi |
|---|---|
| Un template s'écrit **en entier dans le `.hpp`** | Le compilateur doit voir le corps pour fabriquer chaque version |
| Petits templates seulement | Les erreurs de templates sont longues à lire |
| Préférer une interface (`virtual`) quand on veut **changer d'implémentation à l'exécution** | Voir [[12 Héritage et interfaces]] |

> [!tip] Lire une erreur de template
> Cherche la **première** ligne qui mentionne **ton** fichier (pas un fichier de la STL) : c'est là que tu as passé le mauvais type.

---

## 🔗 Liens

- [[02 Types forts]] — le template le plus utile du projet
- [[12 Héritage et interfaces]] — la suite
