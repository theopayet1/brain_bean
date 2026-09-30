---
tags:
  - projet/csharp
  - type/index
  - techno/docker
  - techno/dotnet
  - statut/a-jour
aliases:
  - CSharp — Docker
cree: 2026-09-30
maj: 2026-09-30
---

# Docker et .NET

> [!abstract] En une phrase
> **Docker** fait tourner une application dans une **boîte isolée** (un **conteneur**) qui contient tout ce dont elle a besoin. La base de données et l'API tournent ainsi de la même façon sur tous les postes, et au plus près de la prod.

---

## 📦 La métaphore : le conteneur de bateau

> [!example] Le conteneur de marchandises
> Peu importe le bateau ou le camion, un conteneur standard se charge partout de la même façon. Une **image**, c'est le **modèle** du conteneur. Un **conteneur**, c'est une boîte **fabriquée à partir de ce modèle** et en train de tourner.

---

## 🧩 Les mots à connaître

| Mot | Ce que ça veut dire |
|---|---|
| **Image** | Le modèle figé : le code compilé et le runtime .NET, ou une base de données toute prête (`mariadb:11.4`) |
| **Conteneur** | Une image en train de tourner |
| **Contexte de build** | Le dossier envoyé à Docker pour construire l'image. Les chemins du Dockerfile partent de là |
| **Volume** | Un espace disque qui **survit** au conteneur : les données de la base y sont gardées |
| **Port publié** | `6400:8080` : le port 6400 du PC renvoie vers le port 8080 du conteneur |
| **`.dockerignore`** | Ce qui n'est **jamais** envoyé dans l'image, voir [[02 Garder les secrets hors du repo]] |

---

## 📂 `Dockerfile` ou `docker-compose.yml` ?

| Fichier | Rôle | Où le ranger |
|---|---|---|
| **`Dockerfile`** | La recette qui construit **une** image (celle de l'API) | Dans le projet concerné : `MonApi.Api/Dockerfile` |
| **`docker-compose.yml`** | Orchestre **plusieurs** services : base, API, reverse proxy… | À la **racine** de la solution |

> [!tip] Commencer par la base seule
> Un `docker-compose.yml` peut ne lancer que la base au début, et accueillir l'API (puis un reverse proxy) plus tard.

---

## 📚 Notes de la section

| Note | Contenu |
|---|---|
| [[01 Docker Compose — base de données et API]] | La base seule, puis la base et l'API ensemble, avec le `.env` |

---

## 🔗 Liens

- [[csharp]] — accueil du vault
- [[01 Rider — lancer l'API en local ou dans Docker]] — lancer Compose depuis l'IDE
