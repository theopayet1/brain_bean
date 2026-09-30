---
tags:
  - projet/csharp
  - type/index
  - techno/csharp
  - statut/brouillon
aliases:
  - CSharp — le langage
cree: 2026-09-30
maj: 2026-09-30
---

# Le langage C#

> [!abstract] En une phrase
> Cette section regroupera les notes sur **le langage C# lui-même** : les types, les classes, `async`/`await`, LINQ… Elle est vide pour l'instant, les premières notes du vault portent sur l'architecture et ASP.NET Core.

---

## 🧩 Déjà croisé dans les autres notes

| Notion | Où on l'a vue |
|---|---|
| **`record`** | `Result<T>` et les DTOs, voir [[03 Résultat d'un cas d'usage (Result)]] |
| **Types nullables** (`string?`, `Guid?`) | Une colonne facultative, voir [[03 Synchroniser des données externes (upsert)]] |
| **`async` / `await`** et `CancellationToken` | Tous les repositories et services |
| **Méthodes d'extension** | `AddRepositories()`, voir [[02 Injection de dépendances en .NET]] |
| **`??` et `throw`** | `GetConnectionString("Default") ?? throw …` |
| **`internal`** | Les classes cachées dans leur projet, voir [[00 API tierces]] |

---

> [!todo] À compléter
> - [ ] Les types valeur et référence
> - [ ] Les classes, les propriétés, les constructeurs, les `record`
> - [ ] `async` / `await` et les `Task`
> - [ ] LINQ
> - [ ] Les types nullables et le mot-clé `?`
> - [ ] Les conventions de code (PascalCase, `_champPrive`)

---

## 🔗 Liens

- [[csharp]] — accueil du vault
- [[00 Architecture .NET]] — la suite : organiser une API
