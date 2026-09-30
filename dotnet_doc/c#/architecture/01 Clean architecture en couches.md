---
tags:
  - projet/csharp
  - type/archi
  - techno/csharp
  - techno/dotnet
  - sujet/architecture
  - statut/a-jour
aliases:
  - Clean architecture
  - Architecture en couches
cree: 2026-09-30
maj: 2026-09-30
---

# Clean architecture en couches

> [!abstract] En une phrase
> Chaque projet de la solution a **un seul rôle**, et chaque implémentation est cachée derrière une **interface** rangée dans un projet `.Contracts`. Le code ne manipule que des interfaces : on peut donc changer une implémentation, ou la remplacer par une fausse dans un test, sans rien toucher d'autre. À lire avant : [[00 Architecture .NET]].

---

## 🧩 Ce qui va dans chaque projet

| Projet | Contenu | Exemple |
|---|---|---|
| `MonApi.Domain` | Les **entités** : des classes « bêtes », seulement des propriétés, **aucune dépendance** | `User`, `Order` |
| `MonApi.EntitiesContext` | Le `DbContext`, un `DbSet` par entité, une **classe de configuration** par entité, les **migrations** | `MonApiDbContext`, `UserConfiguration` |
| `MonApi.Repository.Contracts` | Les **interfaces** des repositories | `IUserRepository` |
| `MonApi.Repository` | Les **requêtes concrètes**, écrites avec le `DbContext` | `UserRepository` |
| `MonApi.Application.Contracts` | Les **interfaces** des services métier, leurs **DTOs** et leurs codes d'erreur | `IAuthService`, `Auth/Dtos/LoginRequest` |
| `MonApi.Application` | Les **cas d'usage** : se connecter, créer une commande… | `AuthService` |
| `MonApi.Security` | **Projet-feuille** : hacher un mot de passe, fabriquer un JWT | `BCryptPasswordHasher`, `JwtTokenGenerator` |
| `MonApi.Api` | Les **controllers**, le point d'entrée, le câblage de l'injection de dépendances | `AuthController`, `Program.cs` |

> [!info] Définition — Application, pas « Business »
> **`Application`** est le nom standard en Clean Architecture .NET pour la couche des cas d'usage. « Business » est trop vague : cette couche **orchestre** les actions de l'appli.

---

## 🔗 Qui a le droit de connaître qui

| Projet | Référence |
|---|---|
| `Domain` | rien |
| `Security` | **rien** (projet-feuille) |
| `EntitiesContext` | `Domain` |
| `Repository.Contracts` | `Domain` |
| `Repository` | `Repository.Contracts`, `EntitiesContext`, `Domain` |
| `Application.Contracts` | `Domain` |
| `Application` | `Application.Contracts`, `Repository.Contracts`, `Security`, `Domain` |
| `Api` | tout, **mais seulement pour le câblage** |

> [!warning] `Application` ne connaît pas `Repository`
> `Application` dépend de **`Repository.Contracts`** (les interfaces), jamais de `Repository` (les implémentations). C'est ce qui permet de tester un service avec de faux repositories, voir [[01 Tester sans dépendances externes]].

> [!tip] L'Api référence tout, mais n'utilise que les interfaces
> `Api` doit connaître les implémentations **uniquement pour les enregistrer** dans l'injection de dépendances. Dans le code des controllers, on ne voit que des interfaces (`IAuthService`).

---

## 🍃 Le projet-feuille

Un **projet-feuille** ne référence **aucun** autre projet de la solution. Il rend un service **purement technique** et réutilisable.

`MonApi.Security` en est l'exemple type : il sait hacher un mot de passe et fabriquer un jeton, mais il ne connaît ni les entités, ni la base, ni les controllers. Pour recevoir les infos du jeton, il a **ses propres modèles** plutôt que les entités du Domain :

```csharp
// Modèle propre à Security : le projet reste une feuille.
public record TokenPayload(Guid UserId, string Email, string Role);
```

👉 Un **client d'API tierce** se range de la même façon, dans son propre projet-feuille : voir [[00 API tierces]].

---

## 📏 Conventions

| Règle | Exemple |
|---|---|
| **Namespace = dossier** | `Domain/Accounts/User.cs` → `namespace MonApi.Domain.Accounts;` |
| Entités rangées par **domaine fonctionnel** | `Domain/Accounts`, `Domain/Orders`, `Domain/Billing` |
| Les DTOs dans un sous-dossier `Dtos/` de leur domaine | `Application.Contracts/Auth/Dtos/LoginRequest.cs` |
| **Jamais une entité renvoyée à l'extérieur** | Un controller renvoie toujours un DTO, jamais un `User` |
| Tables et colonnes en anglais, commentaires en français | `user`, `password_hash` / `// Empreinte du mot de passe` |
| Contraintes **hors** des entités | `HasMaxLength`, `IsUnique` dans la config EF, voir [[01 Configurer les entités (IEntityTypeConfiguration)]] |

---

## 🔗 Liens

- [[02 Injection de dépendances en .NET]] — comment chaque couche enregistre ses classes
- [[03 Résultat d'un cas d'usage (Result)]] — ce que renvoie un service de `Application`
- [[04 Architecture (Screen - ViewModel - Model)]] — la même idée de séparation côté Android *(lien valable dans brain_bean)*
