---
tags:
  - projet/csharp
  - type/guide
  - techno/csharp
  - techno/dotnet
  - techno/docker
  - techno/git
  - sujet/securite
  - sujet/configuration
  - statut/a-jour
aliases:
  - Secrets et gitignore .NET
cree: 2026-09-30
maj: 2026-09-30
---

# Garder les secrets hors du repo

> [!abstract] En une phrase
> Un **secret** (mot de passe, clé de signature, token d'API) ne doit jamais être commité ni embarqué dans une image Docker. En dev, on le range dans les **user-secrets**. Dans Docker et en prod, on le passe par **variables d'environnement**. À lire avant : [[01 Configuration et environnements]].

---

## 🧩 Secret ou pas secret ?

| Donnée | Secret ? | Où la mettre |
|---|---|---|
| URL d'un service externe | Non | `appsettings.json` ou variable d'environnement |
| Identifiants de la base **locale Docker** (`monapi_dev_only`) | Non : sans valeur hors du poste | `docker-compose.yml` versionné |
| Chaîne de connexion avec mot de passe réel | **Oui** | User-secrets / variables d'environnement |
| Clé de signature JWT | **Oui** | Idem |
| Token d'une API tierce | **Oui** | Idem |

---

## 🔐 En dev : les user-secrets

Les **user-secrets** sont un magasin de secrets **hors du dossier du projet**, dans le profil Windows (`%APPDATA%\Microsoft\UserSecrets\`). .NET les lit tout seul **en environnement `Development`**.

```bash
dotnet user-secrets init --project MonApi.Api
```

```bash
dotnet user-secrets set "ConnectionStrings:Default" "Server=localhost;Port=3306;Database=monapi;User=monapi;Password=dev_only;" --project MonApi.Api
```

Générer et enregistrer une clé JWT (PowerShell) :

```powershell
$key = [Convert]::ToBase64String((1..48 | ForEach-Object { Get-Random -Maximum 256 }))
dotnet user-secrets set "Jwt:SigningKey" $key --project MonApi.Api
```

| Point | Pourquoi |
|---|---|
| Hors du repo **par construction** | Impossible de les commiter par erreur |
| `"ConnectionStrings:Default"` | Le `:` sépare les niveaux, et le nom doit être le même que dans `GetConnectionString("Default")` |
| 48 octets aléatoires en Base64 | Une clé HMAC-SHA256 doit faire **au moins 32 caractères** |

> [!warning] Par utilisateur et par machine
> Les user-secrets sont à **refaire sur chaque nouveau poste**. Le README du projet doit lister les commandes.

> [!failure] Pas dans une run configuration Rider
> Ne **jamais** mettre une clé dans les variables d'environnement d'une configuration de lancement Rider : elles sont stockées dans `.idea/`, qui peut partir dans git.

---

## 🐳 Dans Docker : variables d'environnement et `.env`

Dans un conteneur, les user-secrets n'existent pas. On passe les secrets en **variables d'environnement**, remplies par un fichier **`.env`** que Docker Compose lit tout seul. Voir [[01 Docker Compose — base de données et API]].

---

## 🙈 Le `.gitignore`

```gitignore
# Réglages locaux et secrets
*.local.json
.env
```

> [!warning] Le `.gitignore` n'agit pas sur un fichier déjà suivi
> Si un fichier est déjà dans le repo, l'ajouter au `.gitignore` ne suffit pas. Il faut dire à git d'arrêter de le suivre, **sans le supprimer du disque** :
> ```bash
> git rm --cached chemin/du/fichier.json
> ```

Pour vérifier qu'un fichier est bien ignoré, et par quelle règle :

```bash
git check-ignore -v chemin/du/fichier.json
```

---

## 🐳 Le `.dockerignore` : l'autre porte de sortie

Un Dockerfile fait souvent `COPY . .` : il copie **tout le dossier** dans l'image, et `dotnet publish` recopie ensuite tous les `appsettings*.json`. Docker **ne lit pas le `.gitignore`** : sans règle, un fichier de secrets local finirait dans l'image.

```dockerignore
**/.env
**/*.local.json
```

> [!example] Deux portes, deux serrures
> - Le **`.gitignore`** ferme la porte vers **git** (GitHub, GitLab).
> - Le **`.dockerignore`** ferme la porte vers **l'image Docker** et le registry où elle sera poussée.
>
> Il faut fermer les deux.

---

## ⚠️ Bonnes pratiques

> [!success] À faire
> - Vérifier le `.gitignore` **avant** d'écrire le secret dans un fichier.
> - Versionner un **modèle** avec des valeurs factices (`.env.example`) pour que l'équipe sache quoi remplir.
> - Donner à un token d'API tierce **le moins de droits possible** (utilisateur dédié, lecture seule).
> - Envoyer un token dans le header `Authorization: Bearer …`, jamais dans l'URL.

> [!failure] À éviter
> - Écrire un secret dans un log ou dans un message d'erreur.
> - Croire qu'un secret supprimé d'un fichier a disparu : il reste dans **l'historique git**. Un secret déjà poussé doit être **changé**.

---

## 🔗 Liens

- [[01 Configuration et environnements]] — l'ordre de priorité des sources
- [[01 Docker Compose — base de données et API]] — le `.env` lu par Compose
- [[01 Rider — lancer l'API en local ou dans Docker]] — le piège de `.idea/`
