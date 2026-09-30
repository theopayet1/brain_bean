---
tags:
  - projet/csharp
  - type/concept
  - techno/csharp
  - techno/dotnet
  - sujet/synchronisation
  - statut/a-jour
aliases:
  - BackgroundService
  - Hosted service
cree: 2026-09-30
maj: 2026-09-30
---

# Tâches de fond (BackgroundService)

> [!abstract] En une phrase
> Un **`BackgroundService`** est une classe qui tourne **toute seule, à côté des requêtes HTTP**, du démarrage à l'arrêt de l'API. Exemple : synchroniser une API tierce toutes les 60 secondes. À lire avant : [[02 Injection de dépendances en .NET]].

---

## 🧩 Le principe

> [!example] Le veilleur de nuit
> Les controllers sont les **guichetiers** : ils ne travaillent que quand un client se présente. Le `BackgroundService` est le **veilleur** qui fait sa ronde toutes les 60 secondes, même si personne ne passe.

On l'enregistre une fois (dans le projet `Api`), et l'hôte le démarre et l'arrête tout seul :

```csharp
services.AddHostedService<ZabbixSyncWorker>();
```

---

## 🔁 La boucle

```csharp
protected override async Task ExecuteAsync(CancellationToken stoppingToken)
{
    if (!this._options.Enabled)
    {
        return;
    }

    using PeriodicTimer timer = new PeriodicTimer(TimeSpan.FromSeconds(this._options.IntervalSeconds));
    try
    {
        do
        {
            await this.SynchronizeAsync(stoppingToken);
        }
        while (await timer.WaitForNextTickAsync(stoppingToken));
    }
    catch (OperationCanceledException) when (stoppingToken.IsCancellationRequested)
    {
        // Arrêt normal de l'application.
    }
}
```

| Code | Pourquoi |
|---|---|
| `ExecuteAsync(CancellationToken stoppingToken)` | La méthode appelée au démarrage. Le jeton est annulé quand l'appli s'arrête |
| `if (!Enabled) return;` | La tâche peut être coupée par la config : elle se termine tout de suite |
| `PeriodicTimer` | Un minuteur qui « sonne » toutes les N secondes, sans chevauchement |
| `do { … } while (…)` | Un premier passage **tout de suite**, puis un à chaque tic |
| `catch (OperationCanceledException) when …` | L'arrêt de l'appli annule l'attente : ce n'est pas une erreur |

---

## ⚠️ Les deux pièges

### Un service scoped dans un singleton

Un `BackgroundService` vit **aussi longtemps que l'appli** : c'est un **singleton**. Le `DbContext` et les repositories sont **scoped** et doivent être recréés à chaque travail. On crée donc **un scope par passage** :

```csharp
using IServiceScope scope = this._scopeFactory.CreateScope();
IZabbixSyncService syncService = scope.ServiceProvider.GetRequiredService<IZabbixSyncService>();
```

> [!warning] Sans scope
> Injecter directement un service scoped dans le constructeur du worker fait planter l'appli au démarrage. Et même sans plantage, on garderait le même `DbContext` pendant des jours.

### Une exception qui arrête toute l'API

Depuis .NET 6, **une exception non attrapée dans `ExecuteAsync` arrête toute l'application**. Si le service externe tombe, l'API tomberait avec lui. On attrape donc tout, sauf l'arrêt normal :

```csharp
catch (ZabbixException exception)
{
    // Service externe indisponible : on réessaie au prochain passage sans arrêter l'application.
    this._logger.LogWarning("Zabbix synchronization failed: {Error}", exception.Message);
}
catch (Exception exception) when (!stoppingToken.IsCancellationRequested)
{
    this._logger.LogError(exception, "Unexpected error during the Zabbix synchronization.");
}
```

> [!tip] Une seule exécution à la fois
> Si la même action peut aussi être déclenchée à la main (une route `POST …/sync`), un verrou (`SemaphoreSlim`) dans le service empêche deux passages de tourner en même temps.

> [!tip] Des logs lisibles
> Une tâche qui tourne toutes les minutes remplit vite la console. Dans `appsettings.json`, passer les catégories bavardes en `Warning` :
> ```json
> "Microsoft.EntityFrameworkCore.Database.Command": "Warning",
> "System.Net.Http.HttpClient": "Warning"
> ```

---

## 🔗 Liens

- [[02 Injection de dépendances en .NET]] — singleton, scoped, transient
- [[03 Zabbix — synchroniser dans sa base]] — un exemple complet de synchronisation
- [[01 Tester sans dépendances externes]] — tester un worker
