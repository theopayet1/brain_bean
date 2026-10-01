---
tags:
  - projet/csharp
  - type/concept
  - techno/csharp
  - techno/efcore
  - sujet/architecture
  - sujet/base-de-donnees
  - statut/a-jour
aliases:
  - EfRepository
  - Pattern Repository
cree: 2026-10-01
maj: 2026-10-01
---

# Repository classique ou repository EF

> [!abstract] En une phrase
> Un **repository** est une classe qui cache l'accès aux données. Dans un repository **classique**, on écrit le **SQL à la main**. Dans un repository **EF**, c'est **Entity Framework** qui écrit le SQL à partir de LINQ. Le préfixe `Ef` dans un nom de classe (`EfUserRepository`) ne fait que **dire quelle technique est utilisée**. À lire avant : [[01 Clean architecture en couches]].

---

## 🧩 C'est quoi un repository

> [!example] Le bibliothécaire
> Tu demandes « le livre de tel auteur » au bibliothécaire. Tu ne sais pas dans quelle salle il est rangé, ni comment il le retrouve. Le **repository**, c'est le bibliothécaire : le service lui demande « l'utilisateur qui a cet email », sans savoir comment la base est interrogée.

Les deux sortes de repository rendent **exactement le même service**. Ce qui change, c'est **comment** ils vont chercher les données.

---

## ✍️ Le repository classique : le SQL à la main

On ouvre la connexion, on écrit la requête SQL, on lit le résultat colonne par colonne. C'est **ADO.NET**, la brique de base de .NET.

```csharp
public class UserRepository : IUserRepository
{
    private readonly string _connectionString;

    public async Task<User?> GetByEmailAsync(string email, CancellationToken cancellationToken = default)
    {
        await using var connection = new SqlConnection(_connectionString);
        await connection.OpenAsync(cancellationToken);

        await using var command = new SqlCommand(
            "SELECT Id, Email, IsActive FROM [User] WHERE Email = @email", connection);
        command.Parameters.AddWithValue("@email", email);

        await using var reader = await command.ExecuteReaderAsync(cancellationToken);
        if (!await reader.ReadAsync(cancellationToken))
            return null;

        return new User
        {
            Id = reader.GetGuid(0),
            Email = reader.GetString(1),
            IsActive = reader.GetBoolean(2)
        };
    }
}
```

| Code | Pourquoi |
|---|---|
| `new SqlConnection(…)` | On ouvre soi-même la connexion à la base |
| `"SELECT … WHERE Email = @email"` | Le **SQL est écrit à la main**, dans une chaîne de caractères |
| `Parameters.AddWithValue(…)` | Le paramètre est passé à part, jamais collé dans la chaîne (sinon : injection SQL) |
| `reader.GetGuid(0)`… | On recopie **chaque colonne** dans l'objet, à la main : c'est le **mapping** |

> [!tip] Dapper
> **Dapper** est une petite bibliothèque qui garde le SQL à la main mais fait le mapping toute seule : `connection.QuerySingleOrDefaultAsync<User>(sql, new { email })`. C'est toujours un repository « classique ».

---

## 🤖 Le repository EF : le SQL écrit par Entity Framework

On écrit du **LINQ** sur le `DbContext`, et EF Core le traduit en SQL.

```csharp
public class EfUserRepository : IUserRepository
{
    private readonly MonApiDbContext _dbContext;

    public EfUserRepository(MonApiDbContext dbContext) => _dbContext = dbContext;

    public Task<User?> GetByEmailAsync(string email, CancellationToken cancellationToken = default)
        => _dbContext.Users.FirstOrDefaultAsync(user => user.Email == email, cancellationToken);
}
```

| Code | Pourquoi |
|---|---|
| `MonApiDbContext` | EF gère la connexion, on ne l'ouvre plus soi-même |
| `user => user.Email == email` | Du **C#** vérifié à la compilation : une faute dans un nom de colonne ne compile pas |
| `FirstOrDefaultAsync(…)` | EF **écrit le SQL** et **remplit l'objet** tout seul |

---

## ⚖️ La comparaison

| | Repository classique (SQL à la main) | Repository EF |
|---|---|---|
| Qui écrit le SQL | Toi | EF Core, à partir du LINQ |
| Remplir les objets (mapping) | À la main, ou avec Dapper | Automatique |
| Faute de frappe dans une colonne | Découverte **à l'exécution** | Découverte **à la compilation** |
| Modifier un objet | Écrire un `UPDATE` | Changer la propriété puis `SaveChangesAsync()` : c'est le **suivi des changements** |
| Créer et faire évoluer les tables | Scripts SQL à maintenir | **Migrations** générées depuis les entités |
| Changer de base (SQL Server → MariaDB) | Réécrire le SQL qui diffère | Changer le fournisseur |
| Contrôle sur la requête | **Total** | Bon, mais il faut surveiller le SQL généré |
| Quantité de code | Beaucoup | Peu |
| Tests | Il faut une vraie base | Base **InMemory** possible |

> [!warning] Le piège d'EF
> Comme on ne voit pas le SQL, on peut écrire sans s'en rendre compte une requête très lente (charger toute une table, puis filtrer en C#). Voir les requêtes générées de temps en temps reste utile.

---

## 🏷️ Le préfixe `Ef` n'est qu'un nom

`UserRepository` et `EfUserRepository` peuvent être **exactement la même classe** : si les deux utilisent un `DbContext`, ce sont deux repositories EF. Le préfixe est une **convention de nommage**, pas une technique différente.

| Convention | Exemple | Idée |
|---|---|---|
| Sans préfixe | `UserRepository` dans le projet `MonApi.Repository` | C'est le **projet** qui dit la technique : tout ce projet utilise EF |
| Avec préfixe | `EfUserRepository` | C'est le **nom** qui dit la technique |

Le préfixe devient vraiment utile quand **plusieurs implémentations du même contrat coexistent** :

```csharp
EfUserRepository        // lit dans la base avec EF Core
DapperUserRepository    // lit dans la base avec du SQL à la main
InMemoryUserRepository  // garde tout en mémoire, pour des tests ou une démo
ZabbixHostRepository    // lit dans une API tierce au lieu d'une base
```

👉 Toutes implémentent la même interface (`IUserRepository`, `IHostRepository`). Le service qui les utilise **ne voit aucune différence** : on choisit laquelle brancher dans l'injection de dépendances, voir [[02 Injection de dépendances en .NET]].

> [!success] La règle
> Choisir **une** convention par solution et s'y tenir. Mélanger `UserRepository` et `EfOrderRepository` dans le même projet laisse croire que les deux ne fonctionnent pas pareil.

---

## 🧭 Lequel choisir

| Situation | Choix |
|---|---|
| Une API classique : créer, lire, modifier, supprimer | **Repository EF** : moins de code, migrations, compilation qui vérifie |
| Une requête très lourde ou très particulière (rapport, statistiques) | **SQL à la main** (Dapper) pour cette requête-là, même dans un projet EF |
| Une base existante au schéma compliqué, qu'on ne contrôle pas | **SQL à la main** |

Les deux peuvent cohabiter : un repository EF pour le quotidien, et une ou deux méthodes en SQL à la main là où la performance compte.

---

## 🔗 Liens

- [[01 Clean architecture en couches]] — la place des repositories et de leurs interfaces
- [[00 EF Core]] — l'outil derrière un repository EF
- [[01 Configurer les entités (IEntityTypeConfiguration)]] — le suivi des changements dans un repository EF
- [[01 Tester sans dépendances externes]] — remplacer un repository par un faux
