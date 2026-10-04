---
tags:
  - projet/cpp
  - type/archi
  - techno/cpp
  - sujet/architecture
  - sujet/injection-de-dependances
  - statut/a-jour
aliases:
  - Ports C++
  - Interfaces de dépôt
cree: 2026-10-04
maj: 2026-10-04
---

# Les ports (interfaces)

> [!abstract] En une phrase
> Un **port** est une interface (classe abstraite C++) rangée dans `application/ports/`, qui dit **ce dont l'application a besoin** du monde extérieur — ranger des livres, connaître la date du jour, choisir un fichier — **sans dire comment**. L'infrastructure fournit la vraie version, les tests une fausse.

---

## 📄 Un port de stockage : `IBooks`

```cpp
#pragma once

#include "domain/book/book.hpp"
#include "domain/common/id.hpp"

#include <optional>
#include <string_view>
#include <vector>

namespace bookshelf::application
{

// Ce dont le service a besoin pour ranger des livres. Il ne sait pas OÙ ils sont rangés :
// SQLite dans l'application, une simple liste en mémoire dans les tests.
class IBooks
{
public:
    IBooks() = default;
    IBooks(const IBooks&) = delete;
    IBooks& operator=(const IBooks&) = delete;
    virtual ~IBooks() = default;

    // Insère le livre (son `id` est ignoré) et renvoie l'identifiant attribué.
    [[nodiscard]] virtual domain::Id<domain::Book> create(const domain::Book& book) = 0;

    [[nodiscard]] virtual std::optional<domain::Book> get(domain::Id<domain::Book> id) = 0;

    // Faux si le livre n'existe pas.
    [[nodiscard]] virtual bool update(const domain::Book& book) = 0;

    // Faux si le livre n'existait pas.
    [[nodiscard]] virtual bool remove(domain::Id<domain::Book> id) = 0;

    // Livres dont le titre ou l'auteur contient `search` (tous si vide), triés par titre.
    [[nodiscard]] virtual std::vector<domain::Book> list(std::string_view search) = 0;
};

} // namespace bookshelf::application
```

| Choix | Pourquoi |
|---|---|
| Préfixe `I` | On voit que c'est une interface |
| Constructeur par défaut + copie `= delete` + destructeur `virtual` | Le gabarit de toute interface, voir [[12 Héritage et interfaces]] |
| `create` renvoie l'`Id` attribué | C'est la base qui le choisit |
| `get` renvoie `std::optional` | « Pas trouvé » est normal |
| `update` / `remove` renvoient `bool` | Le service décide quoi faire si l'élément n'existe pas |
| Le **commentaire** de chaque méthode dit son contrat | « son `id` est ignoré », « faux si… » : c'est ce que les deux implémentations doivent respecter |
| Les méthodes manipulent des **types du domaine** | Jamais de type SQLite ni de JSON dans un port |

> [!warning] Un port ne lève pas pour un cas prévu
> « Livre introuvable » → `std::nullopt` ou `false`. Une exception ne sort d'un port que pour l'**imprévu** (base corrompue, disque plein).

---

## ⏰ Un port technique : `IClock`

```cpp
#pragma once

#include <chrono>

namespace bookshelf::application
{

// La date du jour. Derrière une interface pour que les tests puissent la fixer.
class IClock
{
public:
    IClock() = default;
    IClock(const IClock&) = delete;
    IClock& operator=(const IClock&) = delete;
    virtual ~IClock() = default;

    [[nodiscard]] virtual std::chrono::year_month_day today() const = 0;
};

} // namespace bookshelf::application
```

---

## 🧭 Quels ports créer

| Besoin de l'application | Port | Vraie implémentation | Fausse (tests) |
|---|---|---|---|
| Stocker des livres | `IBooks` | `BooksSqlite` | `FakeBooks` (une `std::map`) |
| La date du jour | `IClock` | `SystemClock` | `FixedClock` |
| Plusieurs écritures en « tout ou rien » | `ITransactions` | Transaction SQLite | Exécute directement |
| Choisir un fichier ou un dossier | `IFilePicker` | Boîte de dialogue Windows | Chemin fixe |
| Écrire un journal | `ILog` | Fichier texte | Liste en mémoire |
| Liste filtrée et triée pour un écran | `IBookReads` (lecture d'écran) | Une requête SQL dédiée | Lignes données à l'avance |

> [!tip] Un port par besoin, pas par table
> Un port décrit un **besoin** de l'application. S'il faut 3 tables pour enregistrer un emprunt, c'est quand même un seul `ILoans::create`.

### Exemple : le port de transactions

```cpp
// Regroupe plusieurs écritures en une seule opération : tout est enregistré, ou rien.
class ITransactions
{
public:
    ITransactions() = default;
    ITransactions(const ITransactions&) = delete;
    ITransactions& operator=(const ITransactions&) = delete;
    virtual ~ITransactions() = default;

    // Exécute `work` dans une transaction : validée si `work` réussit, annulée s'il renvoie
    // une erreur ou lève une exception (qui est alors propagée).
    [[nodiscard]] virtual domain::Result<void>
    run(const std::function<domain::Result<void>()>& work) = 0;
};
```

Voir [[04 Transactions]].

---

## 🔗 Liens

- [[04 Les services (cas d'usage)]] — qui utilise les ports
- [[03 Un dépôt SQLite]] — la vraie implémentation de `IBooks`
- [[02 Tester un service avec des faux]] — la fausse
