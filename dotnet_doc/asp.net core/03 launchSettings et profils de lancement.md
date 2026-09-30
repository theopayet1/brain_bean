---
tags:
  - projet/csharp
  - type/concept
  - techno/csharp
  - techno/dotnet
  - techno/rider
  - sujet/configuration
  - statut/a-jour
aliases:
  - launchSettings.json
cree: 2026-09-30
maj: 2026-09-30
---

# launchSettings et profils de lancement

> [!abstract] En une phrase
> `Properties/launchSettings.json` décrit **comment lancer l'API depuis l'IDE** : les ports et l'environnement. Il n'est lu **que** par les outils de lancement. Ni `dotnet ef`, ni Docker ne le lisent. À lire avant : [[01 Configuration et environnements]].

---

## 🧩 Ce qu'il contient

Un **profil** = une façon de lancer l'appli.

```json
"http": {
  "commandName": "Project",
  "launchBrowser": false,
  "applicationUrl": "http://localhost:5034",
  "environmentVariables": {
    "ASPNETCORE_ENVIRONMENT": "Development"
  }
}
```

| Ligne | Pourquoi |
|---|---|
| `"commandName": "Project"` | Lance le projet directement, sans Docker |
| `"applicationUrl"` | Le ou les ports d'écoute (le profil `https` en a deux : https et http) |
| `"ASPNETCORE_ENVIRONMENT": "Development"` | Choisit l'environnement, donc `appsettings.Development.json` et les user-secrets |

---

## 🔍 Qui le lit, qui ne le lit pas

| Outil | Lit `launchSettings.json` ? |
|---|---|
| Rider, Visual Studio (bouton ▶) | ✅ Oui |
| `dotnet run` | ✅ Oui (premier profil, ou `--launch-profile http`) |
| **`dotnet ef`** (migrations) | ❌ **Non** : il utilise `Development` par défaut |
| Docker / Docker Compose | ❌ Non : l'environnement vient du `docker-compose.yml` |
| Tests unitaires | ❌ Non |

> [!warning] Le piège de `dotnet ef`
> Si l'IDE lance l'API dans un autre environnement que `Development`, `dotnet ef` ne le sait pas : il ne charge pas les mêmes fichiers et peut planter faute de configuration. On lui passe alors l'environnement **après `--`** :
> ```bash
> dotnet ef database update --project MonApi.EntitiesContext --startup-project MonApi.Api -- --environment Staging
> ```
> Tout ce qui suit `--` est transmis à l'appli. Voir [[02 Migrations et dotnet ef]].

---

## 🔗 Liens

- [[01 Rider — lancer l'API en local ou dans Docker]] — choisir le bon profil
- [[02 Migrations et dotnet ef]] — l'environnement vu par `dotnet ef`
- [[01 Configuration et environnements]] — ce que change l'environnement
