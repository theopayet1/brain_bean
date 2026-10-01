---
tags:
  - projet/csharp
  - type/guide
  - techno/csharp
  - techno/nunit
  - techno/efcore
  - sujet/tests
  - statut/a-jour
aliases:
  - Mocks et fakes CSharp
cree: 2026-09-30
maj: 2026-10-01
---

# Tester sans dépendances externes

> [!abstract] En une phrase
> Un test unitaire ne doit dépendre **ni d'une API tierce, ni de la base, ni de l'heure qu'il est**. On remplace chaque dépendance par un **double** : un **mock** Moq, une **base InMemory** ou un **faux serveur HTTP**. À lire avant : [[00 Tests .NET]].

---

## 🧩 Quel double pour quelle dépendance

| Dépendance | Double |
|---|---|
| Un repository (interface de `Repository.Contracts`) | **Mock Moq** |
| La base de données (pour tester un repository) | **EF Core InMemory** |
| Une API HTTP externe | **Faux `HttpMessageHandler`** |
| L'heure (`TimeProvider`) | **Mock Moq** |
| Une tâche de fond | `StartAsync` / `StopAsync` + `TaskCompletionSource` |

---

## 🎭 Mock Moq : un faux repository

```csharp
this._repository.Setup(repository => repository.UpsertHostsAsync(It.IsAny<IReadOnlyCollection<Host>>()))
    .ReturnsAsync((IReadOnlyCollection<Host> hosts) =>
    {
        this._savedHosts = hosts.ToArray();
        return CreateIds(hosts.Select(host => host.ExternalId!), 200);
    });
```

| Code | Pourquoi |
|---|---|
| `Setup(… It.IsAny<…>())` | « Quand on appelle cette méthode, avec n'importe quel argument… » |
| `this._savedHosts = …` | …on **garde** ce que le service a envoyé, pour le vérifier ensuite |
| `CreateIds(…, 200)` | …et on répond des Id connus (200, 201…), pour vérifier les liens entre objets |

Vérifier ensuite qu'un appel a eu lieu, avec les bons arguments :

```csharp
this._repository.Verify(repository => repository.DisableHostsExceptAsync(
    It.Is<IReadOnlyCollection<string>>(ids => ids.SequenceEqual(new[] { "10084" }))), Times.Once);
```

---

## 🕐 Figer l'heure

```csharp
Mock<TimeProvider> timeProvider = new Mock<TimeProvider>();
timeProvider.Setup(provider => provider.GetUtcNow()).Returns(new DateTimeOffset(Now));
```

👉 Le service reçoit toujours la même heure : on peut vérifier les dates qu'il écrit.

---

## 🌐 Faux serveur HTTP

On passe au `HttpClient` un **handler** maison qui répond à la place du vrai service, et qui garde une copie de chaque requête reçue :

```csharp
this._handler.RespondWithResult("host.get",
    "[{\"hostid\":\"10084\",\"name\":\"srv-web-01\",\"status\":\"1\"}]");

ZabbixApiClient client = new ZabbixApiClient(
    new HttpClient(this._handler, disposeHandler: false),
    Options.Create(new ZabbixOptions { Url = "https://zabbix.example.com", ApiToken = "secret-token" }),
    NullLogger<ZabbixApiClient>.Instance);
```

| Code | Pourquoi |
|---|---|
| `RespondWithResult("host.get", …)` | Le faux serveur répond ce JSON pour cette méthode |
| `new HttpClient(this._handler, …)` | Le client croit parler à Internet, mais tout passe par le handler |
| `Options.Create(…)` | Crée un `IOptions<T>` sans fichier de configuration |
| `NullLogger<…>.Instance` | Un logger qui ne fait rien |

Ce qu'on peut vérifier ainsi, sans jamais appeler le vrai service :
- l'adresse appelée et le header `Authorization: Bearer …` ;
- qu'une erreur renvoyée par le service devient une exception propre, **sans le token dans le message** ;
- qu'une panne réseau (`HttpRequestException`) est bien gérée.

---

## 🗄️ Base InMemory

```csharp
DbContextOptions<MonApiDbContext> options = new DbContextOptionsBuilder<MonApiDbContext>()
    .UseInMemoryDatabase(Guid.NewGuid().ToString())
    .Options;
```

👉 `Guid.NewGuid()` donne une **base neuve à chaque test** : aucun test ne voit les données d'un autre.

> [!warning] Limites
> InMemory n'applique pas les index uniques et ne supporte pas `ExecuteUpdateAsync`. Voir [[03 Synchroniser des données externes (upsert)]].

---

## ⏱️ Tester une tâche de fond

```csharp
TaskCompletionSource synchronized = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);
this._syncService.Setup(service => service.SynchronizeAsync(It.IsAny<CancellationToken>()))
    .Returns(Task.CompletedTask)
    .Callback(() => synchronized.TrySetResult());

await worker.StartAsync(CancellationToken.None);
await synchronized.Task.WaitAsync(TimeSpan.FromSeconds(5));
await worker.StopAsync(CancellationToken.None);
```

| Code | Pourquoi |
|---|---|
| `TaskCompletionSource` | Un « signal » que le test attend, sans `Thread.Sleep` hasardeux |
| `.Callback(() => synchronized.TrySetResult())` | Le faux service allume le signal quand le worker l'appelle |
| `WaitAsync(TimeSpan.FromSeconds(5))` | Si le worker n'appelle jamais, le test échoue au bout de 5 s au lieu de bloquer |
| `worker.ExecuteTask.IsCompletedSuccessfully` | Vérifie que la tâche s'arrête proprement, même après une panne |

---

## 🔗 Liens

- [[00 Tests .NET]] — les conventions
- [[02 Injection de dépendances en .NET]] — pourquoi tout est remplaçable
- [[02 Zabbix — l'API JSON-RPC]] — le client testé ici
