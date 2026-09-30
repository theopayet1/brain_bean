---
tags:
  - projet/csharp
  - type/concept
  - techno/csharp
  - techno/dotnet
  - sujet/injection-de-dependances
  - sujet/architecture
  - statut/a-jour
aliases:
  - DI .NET
cree: 2026-09-30
maj: 2026-09-30
---

# Injection de dépendances en .NET

> [!abstract] En une phrase
> .NET a un **conteneur d'injection de dépendances** intégré : au démarrage, on déclare « quand quelqu'un demande `IUserRepository`, donne-lui un `UserRepository` », puis chaque classe reçoit ses objets **par son constructeur**. Chaque couche déclare les siennes dans son propre fichier. À lire avant : [[01 Clean architecture en couches]].

---

## 🗂️ Un `DependencyInjection.cs` par couche

Chaque projet expose **une méthode d'extension** qui enregistre ses propres classes. `Program.cs` se contente de les appeler : il reste court, et ne connaît aucun détail technique.

```csharp
// MonApi.Repository/DependencyInjection.cs
public static class DependencyInjection
{
    public static IServiceCollection AddRepositories(this IServiceCollection services)
    {
        // Scoped : un repository par requête HTTP, aligné sur la durée de vie du DbContext.
        services.AddScoped<IUserRepository, UserRepository>();

        return services;
    }
}
```

```csharp
// MonApi.Api/Program.cs
var builder = WebApplication.CreateBuilder(args);

// --- Configuration ---
var connectionString = builder.Configuration.GetConnectionString("Default")
    ?? throw new InvalidOperationException("Chaîne de connexion 'Default' introuvable (voir user-secrets).");

// --- Services ---
builder.Services.AddControllers();
builder.Services.AddPersistence(connectionString);          // EntitiesContext
builder.Services.AddRepositories();                         // Repository
builder.Services.AddApplication();                          // Application
builder.Services.AddSecurity(builder.Configuration);        // Security

var app = builder.Build();
app.MapControllers();
app.Run();
```

| Code | Pourquoi |
|---|---|
| `AddPersistence`, `AddRepositories`… | Chaque couche possède sa configuration, comme chaque entité possède sa classe de config EF |
| `?? throw new InvalidOperationException(…)` | Si la chaîne de connexion manque, l'appli **plante tout de suite** avec un message clair, au lieu d'échouer plus tard au premier appel : c'est le **fail-fast** |
| `AddSecurity(builder.Configuration)` | La couche reçoit la config et lit elle-même sa section (`Jwt`) |

---

## ⏳ Les trois durées de vie

| Méthode | Un objet… | Pour quoi |
|---|---|---|
| **`AddSingleton`** | **Unique** pour toute la vie de l'appli | Les services **sans état et thread-safe** : hachage de mot de passe, générateur de JWT, `TimeProvider` |
| **`AddScoped`** | **Un par requête HTTP** | Le `DbContext`, et donc les repositories et les services qui l'utilisent |
| **`AddTransient`** | **Neuf à chaque demande** | Les objets légers, comme un client HTTP typé |

> [!warning] Règle d'or
> Un objet ne doit jamais garder un objet qui vit **moins longtemps** que lui. Un singleton qui garderait un `DbContext` le garderait pendant des jours. Voir [[04 Tâches de fond (BackgroundService)]].

---

## 🌐 Le client HTTP typé

Pour appeler une API externe, on ne crée **jamais** de `new HttpClient()` à la main : on laisse .NET le fabriquer et le recycler. C'est le **client HTTP typé**.

```csharp
services.AddHttpClient<IZabbixClient, ZabbixApiClient>((serviceProvider, httpClient) =>
{
    ZabbixOptions options = serviceProvider.GetRequiredService<IOptions<ZabbixOptions>>().Value;
    httpClient.Timeout = TimeSpan.FromSeconds(options.TimeoutSeconds);
});
```

| Code | Pourquoi |
|---|---|
| `AddHttpClient<IZabbixClient, ZabbixApiClient>` | Enregistre l'implémentation avec son propre `HttpClient`, géré par .NET |
| `httpClient.Timeout = …` | Ne pas attendre le service externe indéfiniment |

> [!tip] L'heure aussi s'injecte
> Plutôt que `DateTime.UtcNow`, un service peut recevoir un **`TimeProvider`**. En test, on lui donne une heure fixe et les dates deviennent vérifiables.
> ```csharp
> services.TryAddSingleton(TimeProvider.System);
> ```
> `TryAdd…` n'enregistre que si personne ne l'a déjà fait.

---

## 🔗 Liens

- [[04 Tâches de fond (BackgroundService)]] — créer un scope depuis un singleton
- [[01 Tester sans dépendances externes]] — l'intérêt de tout injecter : pouvoir tout remplacer
- [[08 Koin (injection de dépendances)]] — le même principe en Kotlin *(lien valable dans brain_bean)*
