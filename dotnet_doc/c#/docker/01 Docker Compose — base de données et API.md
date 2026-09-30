---
tags:
  - projet/csharp
  - type/setup
  - techno/docker
  - techno/dotnet
  - techno/mariadb
  - techno/sqlserver
  - sujet/configuration
  - sujet/build
  - statut/a-jour
aliases:
  - docker-compose .NET
cree: 2026-09-30
maj: 2026-09-30
---

# Docker Compose — base de données et API

> [!abstract] En une phrase
> Un `docker-compose.yml` à la racine de la solution lance la **base de données**, puis, quand on veut être au plus près de la prod, l'**API** elle-même. Les secrets de l'API arrivent par un fichier **`.env`** non versionné. À lire avant : [[00 Docker et .NET]].

---

## 🗄️ Étape 1 : la base seule

```yaml
services:
  mariadb:
    image: mariadb:11.4                  # version LTS, stable
    container_name: monapi-mariadb
    restart: unless-stopped
    environment:
      MARIADB_ROOT_PASSWORD: root_dev_only
      MARIADB_DATABASE: monapi           # la base est créée automatiquement
      MARIADB_USER: monapi               # un utilisateur dédié (pas root)
      MARIADB_PASSWORD: monapi_dev_only
    ports:
      - "3306:3306"                      # accessible sur localhost:3306
    volumes:
      - monapi_mariadb_data:/var/lib/mysql   # les données survivent aux redémarrages

volumes:
  monapi_mariadb_data:
```

| Ligne | Pourquoi |
|---|---|
| `image: mariadb:11.4` | Une version **précise** : tout le monde a la même |
| `MARIADB_USER` / `MARIADB_PASSWORD` | Un utilisateur dédié à l'appli, pour ne pas se connecter en `root` |
| `*_dev_only` | Des identifiants **sans valeur hors du poste** : ils peuvent être versionnés |
| `volumes:` | Sans volume, la base est vidée à chaque recréation du conteneur |

L'API tourne alors sur le poste (depuis l'IDE), et se connecte à `localhost:3306` avec la chaîne de connexion rangée dans les user-secrets.

---

## 🌐 Étape 2 : la base et l'API

On ajoute un service `api`, construit à partir du Dockerfile :

```yaml
  api:
    build:
      context: .                         # la racine de la solution
      dockerfile: MonApi.Api/Dockerfile
    environment:
      ASPNETCORE_ENVIRONMENT: Docker
      ConnectionStrings__Default: "Server=mariadb;Port=3306;Database=monapi;User=monapi;Password=monapi_dev_only;"
      Jwt__SigningKey: ${JWT_SIGNING_KEY}
      Zabbix__ApiToken: ${ZABBIX_API_TOKEN}
    ports:
      - "6400:8080"
    depends_on:
      mariadb:
        condition: service_healthy
```

| Ligne | Pourquoi |
|---|---|
| `context: .` | Le Dockerfile copie tous les projets de la solution : il doit partir de la racine |
| `ASPNETCORE_ENVIRONMENT: Docker` | Un environnement dédié, par exemple pour appliquer les migrations au démarrage |
| `Server=mariadb` | Entre conteneurs, on se parle **par le nom du service**, pas par `localhost` |
| `${JWT_SIGNING_KEY}` | Compose remplace `${…}` par la valeur du `.env` : le secret n'est pas écrit dans ce fichier |
| `"6400:8080"` | L'API Docker répond sur **http://localhost:6400** |
| `condition: service_healthy` | L'API attend que la base réponde vraiment (il faut un `healthcheck` sur la base) |

> [!warning] Deux ports différents
> L'API **dans Docker** et l'API **lancée depuis l'IDE** n'ont pas le même port (6400 contre 5034, par exemple). Si « l'API ne répond pas », vérifier d'abord le port.

---

## 🧾 Le `.env`

Docker Compose lit **tout seul** un fichier `.env` placé à côté du `docker-compose.yml`. On versionne un modèle, et chacun le copie :

```bash
# .env.example (versionné) → à copier en .env (jamais commité)
JWT_SIGNING_KEY=
ZABBIX_API_TOKEN=
```

> [!info] Pourquoi un `.env` en .NET ?
> Dans le conteneur, les user-secrets n'existent pas. La config doit donc arriver par **variables d'environnement**, et le `.env` est le moyen standard de Compose pour les remplir.

---

## 🧰 Les commandes

| Besoin | Commande |
|---|---|
| Tout lancer, en reconstruisant l'image de l'API | `docker compose up -d --build` |
| Lancer **seulement** la base | `docker compose up -d mariadb` |
| Voir les conteneurs et leur état | `docker ps` |
| Lire les logs d'un conteneur | `docker logs <nom>` |
| Tout arrêter, **en gardant** les données | `docker compose down` |
| Tout arrêter **et effacer** les données | `docker compose down -v` |
| Vérifier la config finale (variables remplacées) | `docker compose config` |

> [!warning] `-v` efface la base
> `docker compose down -v` supprime les volumes : toutes les données sont perdues. Utile pour repartir d'une base propre, pas pour un arrêt de tous les jours.

> [!tip] `docker exec` depuis Git Bash
> Git Bash transforme les chemins Linux (`/opt/…`) en chemins Windows, et `docker exec` échoue avec `no such file or directory`. Désactiver la conversion avant :
> ```bash
> export MSYS_NO_PATHCONV=1
> ```

---

## 🔗 Liens

- [[02 Garder les secrets hors du repo]] — `.env`, `.dockerignore`
- [[01 Rider — lancer l'API en local ou dans Docker]] — lancer ce fichier depuis Rider
- [[02 Migrations et dotnet ef]] — créer le schéma dans la base
