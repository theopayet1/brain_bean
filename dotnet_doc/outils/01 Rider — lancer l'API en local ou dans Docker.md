---
tags:
  - projet/csharp
  - type/guide
  - techno/rider
  - techno/docker
  - techno/dotnet
  - sujet/debug
  - statut/a-jour
aliases:
  - Lancer une API dans Rider
cree: 2026-09-30
maj: 2026-09-30
---

# Rider — lancer l'API en local ou dans Docker

> [!abstract] En une phrase
> Deux façons de lancer une API depuis Rider : **l'API en local et la base dans Docker**, pour déboguer facilement, ou **tout dans Docker**, pour être au plus près de la prod. À lire avant : [[01 Docker Compose — base de données et API]].

---

## 🧩 Quelle option choisir

| | Option 1 : API en local | Option 2 : tout dans Docker |
|---|---|---|
| Ce qui tourne dans Docker | La base seulement | La base **et** l'API |
| Environnement de l'API | `Development` | `Docker` |
| D'où viennent les secrets | User-secrets | `.env` → variables d'environnement |
| Points d'arrêt | ✅ Directement | Plus compliqué |
| Quand | Écrire et déboguer du code | Vérifier que tout marche comme en prod |

---

## 💻 Option 1 : l'API en local

1. Lancer la base : `docker compose up -d` (ou `docker compose up -d mariadb` si le compose contient aussi l'API).
2. En haut à droite de Rider, choisir le profil **`MonApi.Api: http`** (ou `https`).
3. Cliquer sur 🐞 **Debug**.

> [!failure] Pas de secret dans la configuration de lancement
> Rider permet d'ajouter des variables d'environnement à une configuration de lancement, mais elles sont stockées dans `.idea/`, qui peut partir dans git. Les secrets vont dans les **user-secrets**, voir [[02 Garder les secrets hors du repo]].

---

## 🐳 Option 2 : tout dans Docker

1. Ouvrir `docker-compose.yml`, à la racine (**pas** le Dockerfile).
2. Cliquer sur **▶▶** dans la marge, à côté de `services:`. Rider crée une configuration **Docker Compose**.
3. **Run → Edit Configurations** → la configuration Compose :
   - donner un **nom** clair ;
   - **Modify ▾ → Build → Always**, sinon Rider réutilise l'ancienne image et ignore les modifs de code ;
   - le champ **Services** vide lance **tous** les services. Y mettre seulement la base pour l'option 1.
4. Lancer avec ▶. Les logs sont dans la fenêtre **Services** (`Alt+8`).

> [!tip] « Running 0.0s »
> Si la sortie affiche `Container … Running 0.0s`, Rider n'a **rien reconstruit** : il manque **Build → Always**.

> [!warning] Une seule API à la fois
> Si l'API tourne dans Docker **et** dans Rider, deux instances travaillent sur la même base (et font deux fois les tâches de fond). Arrêter le conteneur de l'API avant de déboguer : `docker stop <nom-du-conteneur>`.

---

## 🧯 Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| `failed to compute cache key: "/MonApi.Infrastructure/…csproj": not found` | Rider a lancé le **Dockerfile seul**, avec le dossier du projet comme contexte de build : les autres projets sont introuvables | Lancer `docker-compose.yml` à la place (`context: .`), et supprimer la configuration *Dockerfile* |
| L'API démarre en Docker mais plante (config, connexion) | La configuration *Dockerfile* ne passe aucune variable d'environnement et ne démarre pas la base | Passer par Compose |
| « L'API ne répond pas » alors que `docker ps` la montre | Mauvais port | Le port Docker n'est pas celui de l'IDE |
| Les modifs de code n'apparaissent pas dans Docker | L'image n'est pas reconstruite | **Build → Always** |

---

## 🔗 Liens

- [[01 Docker Compose — base de données et API]] — le fichier lancé
- [[03 launchSettings et profils de lancement]] — les profils de l'option 1
- [[00 Outils .NET]] — les autres outils
