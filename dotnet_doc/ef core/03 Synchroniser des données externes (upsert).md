---
tags:
  - projet/csharp
  - type/concept
  - techno/csharp
  - techno/efcore
  - sujet/base-de-donnees
  - sujet/synchronisation
  - sujet/api-tierce
  - statut/a-jour
aliases:
  - Upsert EF Core
  - Identifiant externe
cree: 2026-09-30
maj: 2026-09-30
---

# Synchroniser des données externes (upsert)

> [!abstract] En une phrase
> Pour copier régulièrement les données d'un autre système sans créer de doublons, on garde **l'identifiant de l'autre système** dans une colonne, et on fait un **upsert** : on **met à jour** la ligne si elle existe, on l'**insère** sinon. À lire avant : [[01 Configurer les entités (IEntityTypeConfiguration)]].

---

## 🧩 L'identifiant externe

Chaque entité importée a deux identifiants :

| Colonne | Qui la donne | Exemple |
|---|---|---|
| `Id` | Notre base, à l'insertion | `1004` |
| `ZabbixId` (`string?`) | Le système externe | `"10693"` |

> [!info] Pourquoi `string?`
> - **`string`** : beaucoup d'API renvoient leurs identifiants en texte (c'est le cas de Zabbix). Les garder tels quels évite toute conversion.
> - **`?` (nullable)** : une donnée créée à la main dans notre appli n'a pas d'identifiant externe. `null` veut dire « pas importée ».

---

## 🔒 L'index unique filtré

Dans la classe de configuration de l'entité :

```csharp
builder.Property(h => h.ZabbixId).HasMaxLength(32);
builder.HasIndex(h => h.ZabbixId).IsUnique().HasFilter("[ZabbixId] IS NOT NULL");
```

| Code | Pourquoi |
|---|---|
| `HasMaxLength(32)` | Une colonne de texte illimitée ne peut pas être indexée |
| `IsUnique()` | La base **refuse** deux lignes avec le même identifiant externe : le dernier filet contre les doublons |
| `HasFilter("[ZabbixId] IS NOT NULL")` | Sous SQL Server, deux `NULL` comptent comme égaux : sans filtre, on ne pourrait créer qu'**une seule** donnée manuelle. La syntaxe du filtre dépend de la base |

---

## 🔁 L'upsert, dans le repository

```csharp
public async Task<IReadOnlyDictionary<string, int>> UpsertByZabbixIdAsync(IReadOnlyCollection<GroupHost> groupHosts)
{
    string[] zabbixIds = ZabbixIdGuard.GetZabbixIds(groupHosts, group => group.ZabbixId, nameof(groupHosts));
    Dictionary<string, GroupHost> entities = await this._context.GroupHosts
        .Where(group => group.ZabbixId != null && zabbixIds.Contains(group.ZabbixId))
        .ToDictionaryAsync(group => group.ZabbixId!);

    foreach (GroupHost groupHost in groupHosts)
    {
        if (entities.TryGetValue(groupHost.ZabbixId!, out GroupHost? entity))
        {
            entity.Name = groupHost.Name;                     // existe déjà → mise à jour
        }
        else
        {
            _ = this._context.GroupHosts.Add(groupHost);      // nouveau → insertion
            entities.Add(groupHost.ZabbixId!, groupHost);
        }
    }

    _ = await this._context.SaveChangesAsync();
    return entities.ToDictionary(pair => pair.Key, pair => pair.Value.Id);
}
```

| Code | Pourquoi |
|---|---|
| `ZabbixIdGuard.GetZabbixIds(…)` | Vérifie que chaque élément a un identifiant externe, sans doublon, sinon `ArgumentException` |
| `Where(… zabbixIds.Contains(…))` | **Une seule requête** (`WHERE ZabbixId IN (…)`) pour charger toutes les lignes existantes |
| `ToDictionaryAsync(…)` | Retrouver une ligne par son identifiant externe instantanément |
| `entity.Name = …` | EF suit l'objet chargé : il suffit de modifier la propriété, pas besoin de `UPDATE` |
| `Add(…)` | Marque l'objet « à insérer » |
| `SaveChangesAsync()` une seule fois | Tous les `UPDATE` et `INSERT` partent ensemble. La base attribue les `Id` des nouvelles lignes, et EF les recopie dans les objets |
| `return … pair.Value.Id` | Renvoie la correspondance `identifiant externe → Id`, pour relier la suite (un hôte à son groupe) |

> [!success] Idempotent
> Lancer la synchro une fois ou dix fois donne **le même résultat** : l'opération est **idempotente**.

---

## 🗃️ Ne rien supprimer

Quand un objet disparaît du système externe, on **ne supprime pas** la ligne, pour garder l'historique :

| Objet disparu | Ce qu'on fait |
|---|---|
| Une machine surveillée | `IsDisabled = true` |
| Une alerte | `ResolvedAt = date de la synchro` |

Pour des **mesures** sans identifiant externe (CPU, RAM…), on n'ajoute une valeur que si elle est **plus récente** que la dernière stockée, d'au moins N minutes : pas de doublon, et un historique de taille raisonnable.

---

## ⚠️ Les limites de la base InMemory (tests)

| En vraie base | Avec EF Core InMemory |
|---|---|
| `ExecuteUpdateAsync()` (mise à jour en masse) | ❌ Non supporté : charger les lignes puis les modifier |
| Index unique, filtre `HasFilter` | ⚠️ Ignorés : un test InMemory ne détecte pas un doublon |

---

## 🔗 Liens

- [[03 Zabbix — synchroniser dans sa base]] — un exemple complet
- [[00 API tierces]] — appel direct ou synchronisation
- [[01 Tester sans dépendances externes]] — tester ces repositories avec InMemory
