---
tags:
  - projet/csharp
  - type/guide
  - techno/csharp
  - techno/efcore
  - sujet/base-de-donnees
  - statut/a-jour
aliases:
  - IEntityTypeConfiguration
  - Fluent API
cree: 2026-09-30
maj: 2026-09-30
---

# Configurer les entités (IEntityTypeConfiguration)

> [!abstract] En une phrase
> Plutôt qu'un énorme `OnModelCreating`, chaque entité a **sa propre classe de configuration** dans le dossier `Configurations/`, et le `DbContext` les charge toutes automatiquement. C'est la bonne pratique recommandée par Microsoft. À lire avant : [[00 EF Core]].

---

## 🧩 L'entité reste « bête »

L'entité, dans `Domain`, n'a **que des propriétés** : aucune contrainte, aucun attribut EF.

```csharp
// Domain/Accounts/User.cs
namespace MonApi.Domain.Accounts;

public class User
{
    public Guid Id { get; set; }

    // Identifiant de connexion (unique — l'unicité est posée en Fluent API, pas ici).
    public string Email { get; set; } = string.Empty;

    // Empreinte du mot de passe (jamais le mot de passe en clair).
    public string PasswordHash { get; set; } = string.Empty;

    public bool IsActive { get; set; } = true;
    public DateTime CreatedAt { get; set; }
}
```

---

## ⚙️ Sa configuration

```csharp
// EntitiesContext/Configurations/UserConfiguration.cs
public class UserConfiguration : IEntityTypeConfiguration<User>
{
    public void Configure(EntityTypeBuilder<User> builder)
    {
        builder.ToTable("user");
        builder.HasKey(u => u.Id);

        builder.Property(u => u.Email).HasMaxLength(255).IsRequired();
        builder.HasIndex(u => u.Email).IsUnique();   // un email = un seul compte

        builder.Property(u => u.PasswordHash).HasMaxLength(255).IsRequired();
        builder.Property(u => u.CreatedAt).HasDefaultValueSql("CURRENT_TIMESTAMP");
    }
}
```

| Code | Pourquoi |
|---|---|
| `IEntityTypeConfiguration<User>` | Toute la config de `User` au même endroit, dans un seul fichier |
| `ToTable("user")` | Le nom de la table, au singulier |
| `HasMaxLength(255)` | Sans longueur, la colonne serait en texte illimité, impossible à indexer |
| `HasIndex(…).IsUnique()` | La base **refuse** deux comptes avec le même email |
| `HasDefaultValueSql(…)` | La base remplit la date toute seule à l'insertion |

---

## 🗄️ Le DbContext reste minuscule

```csharp
public class MonApiDbContext : DbContext
{
    // Les options (fournisseur, chaîne de connexion) sont injectées depuis l'Api.
    public MonApiDbContext(DbContextOptions<MonApiDbContext> options) : base(options) { }

    public DbSet<User> Users => Set<User>();

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        base.OnModelCreating(modelBuilder);

        // Charge automatiquement toutes les classes IEntityTypeConfiguration du projet.
        modelBuilder.ApplyConfigurationsFromAssembly(typeof(MonApiDbContext).Assembly);
    }
}
```

> [!success] Pourquoi c'est mieux
> Un `DbContext` court et stable, une config isolée par entité, et **pas de conflits git** quand deux personnes travaillent sur deux entités différentes.

---

## 🐍 snake_case automatique

Pour avoir des colonnes `password_hash` sans écrire `HasColumnName` partout, un **seul réglage global** suffit (paquet `EFCore.NamingConventions`), dans le `DependencyInjection.cs` d'`EntitiesContext` :

```csharp
services.AddDbContext<MonApiDbContext>(options =>
    options
        .UseMySql(connectionString, ServerVersion.AutoDetect(connectionString))
        .UseSnakeCaseNamingConvention());
```

---

## 🛡️ Un filtre appliqué à toutes les requêtes

Quand une seule base sert plusieurs clients (**multi-tenant**), chaque ligne porte une colonne `tenant_id`. Oublier un `WHERE tenant_id = …` dans une seule requête ferait fuiter des données. EF permet un **filtre global**, appliqué automatiquement à **toutes** les requêtes :

```csharp
modelBuilder.Entity<Order>()
    .HasQueryFilter(order => order.TenantId == _currentTenantId);
```

👉 Même si un développeur oublie le filtre dans un repository, EF l'ajoute quand même.

---

## 🔁 Le suivi des changements dans un repository

```csharp
public Task<User?> GetByEmailAsync(string email, CancellationToken cancellationToken = default)
    // Pas de AsNoTracking : l'appelant modifie l'entité, on a besoin du suivi.
    => _dbContext.Users.FirstOrDefaultAsync(user => user.Email == email, cancellationToken);

public Task SaveChangesAsync(CancellationToken cancellationToken = default)
    // L'entité est déjà suivie : EF n'écrit que les colonnes modifiées.
    => _dbContext.SaveChangesAsync(cancellationToken);
```

| Règle | Pourquoi |
|---|---|
| `AsNoTracking()` pour une lecture seule | Plus rapide : EF ne garde pas de copie des objets |
| Pas d'`AsNoTracking()` si on va modifier | Sinon EF ne voit pas les changements |
| Pas de `.Update()` sur une entité déjà suivie | `.Update()` forcerait la réécriture de **toutes** les colonnes |

---

## 🔗 Liens

- [[02 Migrations et dotnet ef]] — transformer cette config en tables
- [[03 Synchroniser des données externes (upsert)]] — un index unique filtré
- [[01 Clean architecture en couches]] — pourquoi les entités restent « bêtes »
