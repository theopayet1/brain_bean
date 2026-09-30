---
tags:
  - projet/csharp
  - type/guide
  - techno/csharp
  - techno/efcore
  - sujet/base-de-donnees
  - sujet/debug
  - statut/a-jour
aliases:
  - dotnet ef
cree: 2026-09-30
maj: 2026-09-30
---

# Migrations et dotnet ef

> [!abstract] En une phrase
> Une **migration** décrit un changement de la base (une table, une colonne, un index). On la **génère** à partir des entités et de leur configuration, puis on l'**applique** à la base, avec l'outil en ligne de commande **`dotnet ef`**. À lire avant : [[01 Configurer les entités (IEntityTypeConfiguration)]].

---

## 🧰 Installer l'outil

```bash
dotnet tool install --global dotnet-ef --version 9.*
```

👉 Prendre la **même version majeure** que les paquets EF du projet, voir [[00 EF Core]].

---

## 🧩 Les deux commandes

Depuis la racine de la solution :

```bash
dotnet ef migrations add <NomDeLaMigration> --project MonApi.EntitiesContext --startup-project MonApi.Api
```

```bash
dotnet ef database update --project MonApi.EntitiesContext --startup-project MonApi.Api
```

| Morceau | Pourquoi |
|---|---|
| `migrations add <Nom>` | Compare le modèle au dernier état connu et écrit le fichier de migration |
| `database update` | Applique à la base les migrations pas encore passées |
| `--project MonApi.EntitiesContext` | Le projet qui contient le `DbContext` et le dossier `Migrations` |
| `--startup-project MonApi.Api` | Le projet de **démarrage** : celui qui fournit la configuration (la chaîne de connexion) |

> [!tip] Relire la migration générée
> Toujours ouvrir le fichier avant de l'appliquer : vérifier qu'il ne contient **aucun `DropColumn` ou `DropTable` inattendu**, qui effacerait des données.

> [!info] Appliquer au démarrage
> Dans un conteneur qui part d'une base vide, l'API peut appliquer les migrations elle-même au démarrage, avec `await context.Database.MigrateAsync();`. On le réserve à un environnement dédié (`Docker`), jamais à la prod sans contrôle.

---

## ⚠️ `dotnet ef` exécute `Program.cs`

Pour trouver le `DbContext`, `dotnet ef` **démarre l'API**, en environnement **`Development`**, et **sans lire `launchSettings.json`**. Si une configuration obligatoire manque dans cet environnement, `Program.cs` plante avant d'avoir enregistré le `DbContext`.

Pour lui donner un autre environnement, on le passe **après `--`** (tout ce qui suit est transmis à l'appli) :

```bash
dotnet ef database update --project MonApi.EntitiesContext --startup-project MonApi.Api -- --environment Staging
```

---

## 🧯 Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| `NETSDK1004 : project.assets.json introuvable` | Le projet n'a jamais été restauré : les paquets NuGet ne sont pas téléchargés | `dotnet restore`, puis relancer |
| `Unable to create a 'DbContext'… Unable to resolve service for type DbContextOptions` | `Program.cs` a planté avant d'enregistrer le `DbContext`, souvent à cause d'une config manquante | Vérifier les user-secrets, ou passer `-- --environment …` |
| `An error occurred while accessing the Microsoft.Extensions.Hosting services… Object reference not set` | Même cause : une section de config est `null` | Idem |
| `The Entity Framework tools version 'X' is older than that of the runtime 'Y'` | L'outil `dotnet ef` est plus ancien que les paquets EF | Pas bloquant : `dotnet tool update --global dotnet-ef` |
| Connexion à la base refusée | Le conteneur de la base n'est pas démarré | `docker compose up -d`, puis vérifier avec `docker ps` |

---

## 🔗 Liens

- [[03 launchSettings et profils de lancement]] — pourquoi `dotnet ef` ne voit pas l'environnement de l'IDE
- [[01 Docker Compose — base de données et API]] — démarrer la base avant la migration
- [[02 Garder les secrets hors du repo]] — la chaîne de connexion en user-secrets
