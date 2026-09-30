---
tags:
  - projet/csharp
  - type/index
  - techno/csharp
  - techno/nunit
  - sujet/tests
  - statut/a-jour
aliases:
  - CSharp — tests
cree: 2026-09-30
maj: 2026-09-30
---

# Tests .NET

> [!abstract] En une phrase
> Un **test unitaire** vérifie un petit morceau de code **tout seul**, sans base de données ni réseau. On utilise **NUnit** pour écrire les tests et **Moq** pour remplacer les dépendances par des fausses.

---

## 🧩 Les outils

| Outil | Rôle |
|---|---|
| **NUnit** | Le framework de test : `[Test]`, `[SetUp]`, `Assert.That(…)` |
| **Moq** | Fabrique de **faux objets** (*mocks*) à partir d'une interface |
| **EF Core InMemory** | Une fausse base de données, en mémoire |

```bash
dotnet test
```

---

## 📏 Les conventions

| Règle | Exemple |
|---|---|
| Nom : `Méthode_Condition_RésultatAttendu` | `LoginAsync_WrongPassword_ReturnsInvalidCredentials` |
| Trois blocs commentés : **Arrange, Act, Assert** | `// Arrange` prépare, `// Act` appelle, `// Assert` vérifie |
| `Assert.That(…)` | `Assert.That(result.IsSuccess, Is.False);` |
| Plusieurs vérifications d'un coup | `using (Assert.EnterMultipleScope()) { … }` : toutes les erreurs sont listées, pas seulement la première |
| Un projet de test par projet testé | `MonApi.Application.Tests`, `MonApi.Repository.Tests` |

> [!example] Arrange, Act, Assert
> Une recette de cuisine testée : **Arrange** = préparer les ingrédients, **Act** = cuire, **Assert** = goûter.

> [!tip] Tester une classe `internal`
> Ajouter dans le `.csproj` du projet testé :
> ```xml
> <ItemGroup>
>   <InternalsVisibleTo Include="MonApi.Application.Tests" />
> </ItemGroup>
> ```

---

## 📚 Notes de la section

| Note | Contenu |
|---|---|
| [[01 Tester sans dépendances externes]] | Remplacer la base, une API tierce, l'heure et le temps qui passe |

---

## 🔗 Liens

- [[csharp]] — accueil du vault
- [[01 Clean architecture en couches]] — les `.Contracts` rendent tout remplaçable
