---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - Strong types
  - Id typé
cree: 2026-10-04
maj: 2026-10-04
---

# Types forts

> [!abstract] En une phrase
> Un **type fort** emballe une valeur simple (un entier) dans un type à part, pour que le compilateur **refuse les confusions** : un identifiant de livre ne peut pas être passé là où on attend un identifiant d'emprunt, un montant en centimes ne se mélange pas à un nombre quelconque.

---

## 🐛 Le problème

```cpp
void lend(std::int64_t bookId, std::int64_t memberId);

lend(memberId, bookId);   // ❌ compile parfaitement… et prête le mauvais livre
```

## ✅ L'identifiant typé

```cpp
#pragma once

#include <compare>
#include <cstdint>

namespace bookshelf::domain
{

// Identifiant typé : un Id<Book> ne se confond pas avec un Id<Loan>.
template <typename T>
struct Id
{
    std::int64_t value = 0;

    friend constexpr auto operator<=>(Id, Id) = default;
};

} // namespace bookshelf::domain
```

```cpp
void lend(Id<Book> book, Id<Member> member);

lend(memberId, bookId);   // ✅ erreur de compilation : Id<Member> n'est pas Id<Book>
```

| Code | Pourquoi |
|---|---|
| `template <typename T>` | `T` sert seulement à fabriquer un type différent par entité |
| `std::int64_t value` | Le vrai nombre (celui de la base) |
| `operator<=> = default` | On peut comparer deux `Id<Book>`, les trier, s'en servir comme clé de `std::map` |
| Pas de constructeur | `Id<Book>{42}` ou `{42}` suffit, c'est une struct simple |

Lire la valeur : `id.value`. C'est voulu : on ne « sort » l'entier que dans l'infrastructure (SQL) et le pont (JSON).

---

## 💶 L'argent : des centimes entiers

```cpp
// Montant en centimes. Toujours un entier : un double produirait des écarts d'un centime.
struct Cents
{
    std::int64_t value = 0;

    friend constexpr auto operator<=>(Cents, Cents) = default;
};

constexpr Cents operator+(Cents a, Cents b) { return {a.value + b.value}; }
```

> [!warning] Jamais de `double` pour de l'argent
> `0.1 + 0.2` vaut `0.30000000000000004`. Avec des centimes entiers, `10 + 20` vaut exactement `30`. On ne formate en euros qu'à l'**affichage**.

---

## 🧭 Quand en créer un

| Valeur | Type fort ? |
|---|---|
| Identifiants de base de données | ✅ `Id<T>` |
| Montants | ✅ `Cents` |
| Durées, dates | ✅ Déjà fournis : `std::chrono` ([[03 Dates avec chrono]]) |
| Mot de passe (à effacer de la mémoire) | ✅ Une classe dont le destructeur efface son contenu |
| Un compteur local | ❌ Un `int` suffit |

---

## 🔗 Liens

- [[11 Templates]] — comment `Id<T>` fonctionne
- [[02 Le domaine]] — où vivent ces types
- [[03 Dates avec chrono]] — la suite
