---
tags:
  - projet/csharp
  - type/index
  - techno/csharp
  - techno/efcore
  - sujet/base-de-donnees
  - statut/a-jour
aliases:
  - Entity Framework Core
cree: 2026-09-30
maj: 2026-09-30
---

# EF Core

> [!abstract] En une phrase
> **Entity Framework Core** (EF Core) traduit des **classes C#** en **tables SQL**, et des requêtes **LINQ** en requêtes SQL : c'est un **ORM**. On manipule des objets, EF écrit le SQL. Il vit dans le projet `EntitiesContext`.

---

## 🧩 Les mots à connaître

| Mot | Ce que ça veut dire |
|---|---|
| **ORM** | Un outil qui fait le lien entre les objets du code et les tables de la base |
| **`DbContext`** | La « session » avec la base. Ses options (fournisseur, chaîne de connexion) sont injectées depuis l'Api |
| **`DbSet<T>`** | Une table vue comme une collection : `context.Users` |
| **Code-first** | On écrit les entités C#, et EF **génère** le schéma de la base |
| **Configuration Fluent API** | Les contraintes (longueur, index, relations) écrites en C#, hors des entités |
| **Migration** | Un fichier C# qui décrit **un changement du schéma** |
| **Suivi des changements** (*change tracking*) | EF retient les objets chargés et n'écrit en base **que ce qui a changé**, au `SaveChangesAsync()` |
| **Fournisseur** (*provider*) | Le pilote de la base : `UseSqlServer`, `UseMySql` (Pomelo, pour MariaDB)… |

> [!warning] Aligner les versions
> Tous les paquets EF d'une solution doivent avoir la **même version majeure** que le fournisseur. Si le fournisseur MariaDB (Pomelo) n'existe qu'en version 9, on met aussi `Microsoft.EntityFrameworkCore.Design` et l'outil `dotnet-ef` en 9, même dans un projet .NET 10.

---

## 📚 Notes de la section

| Note | Contenu |
|---|---|
| [[01 Configurer les entités (IEntityTypeConfiguration)]] | Une classe de config par entité, snake_case, filtre global |
| [[02 Migrations et dotnet ef]] | Créer et appliquer une migration, et les erreurs fréquentes |
| [[03 Synchroniser des données externes (upsert)]] | Importer les données d'un autre système sans doublon |

---

## 🔗 Liens

- [[csharp]] — accueil du vault
- [[01 Clean architecture en couches]] — la place d'`EntitiesContext` et de `Repository`
