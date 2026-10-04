---
tags:
  - projet/cpp
  - type/index
  - techno/cpp
  - statut/a-jour
aliases:
  - C++ — moderne
cree: 2026-10-04
maj: 2026-10-04
---

# C++ moderne

> [!abstract] En une phrase
> Les outils du C++20 / C++23 qui rendent le code **sûr et lisible** : renvoyer une erreur sans exception, des types qui empêchent les confusions, des dates sans bibliothèque, des algorithmes au lieu de boucles, et des fils d'exécution sans danger.

---

## 📚 Notes de la section

| Note | Contenu |
|---|---|
| [[01 Gérer les erreurs (expected et exceptions)]] | `std::expected`, un `Result<T>`, des codes d'erreur stables, les exceptions pour l'imprévu |
| [[02 Types forts]] | `Id<Book>`, `Cents` : le compilateur refuse les mélanges |
| [[03 Dates avec chrono]] | `year_month_day`, aujourd'hui, ISO-8601, calculs |
| [[04 Algorithmes et ranges]] | `std::ranges::sort`, `find_if`, `count_if`, projections |
| [[05 Threads et file de tâches]] | `std::jthread`, `mutex`, `condition_variable`, une file de tâches complète |
| [[06 const, constexpr, noexcept et nodiscard]] | Les promesses faites au compilateur |

---

## 🔗 Liens

- [[cpp]] — accueil du vault
- [[00 Construire un projet C++]] — la suite
