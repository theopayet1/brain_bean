---
tags:
  - projet/csharp
  - type/guide
  - techno/zabbix
  - techno/csharp
  - techno/dotnet
  - sujet/api-tierce
  - sujet/supervision
  - statut/a-jour
aliases:
  - API Zabbix
  - Client Zabbix C#
cree: 2026-09-30
maj: 2026-10-01
---

# Zabbix — l'API JSON-RPC

> [!abstract] En une phrase
> L'API Zabbix n'a **qu'une seule adresse** (`/api_jsonrpc.php`) : on lui envoie en `POST` le **nom de la méthode** et ses **paramètres**. C'est le protocole **JSON-RPC**. Le client C# cache ce format derrière une interface simple. À lire avant : [[01 Zabbix — les concepts]].

---

## 🔑 S'authentifier

1. Dans Zabbix : **Users → API tokens → Create API token**, sur un **utilisateur dédié à l'API**, avec des droits en lecture seule.
2. Envoyer le token dans le header :
   ```
   Authorization: Bearer <token>
   ```

> [!info] Définition — JSON-RPC
> Au lieu d'avoir une URL par ressource (`GET /hosts`, `GET /problems`), **tout passe par la même URL**. Le corps de la requête dit quelle méthode appeler. Toutes les requêtes sont des `POST`.

---

## 📨 Une requête, une réponse

```bash
curl -X POST https://zabbix.example.com/api_jsonrpc.php \
  -H "Content-Type: application/json-rpc" \
  -H "Authorization: Bearer $ZABBIX_API_TOKEN" \
  -d '{"jsonrpc":"2.0","method":"host.get","params":{"output":["hostid","name","status"],"selectHostGroups":["groupid"]},"id":1}'
```

```json
{"jsonrpc":"2.0","result":[{"hostid":"10084","name":"srv-web-01","status":"0","hostgroups":[{"groupid":"2"}]}],"id":1}
```

| Champ | Rôle |
|---|---|
| `"method"` | La méthode appelée : `objet.action` (`host.get`, `problem.get`…) |
| `"params"` | Les paramètres de la méthode |
| `"output"` | Les champs voulus : on ne demande que le nécessaire |
| `"select…"` | Ajoute des objets liés (`selectHostGroups` : les groupes de chaque hôte) |
| `"id"` | Un numéro pour relier la réponse à la requête |
| `"result"` / `"error"` | La réponse contient **l'un ou l'autre** |

> [!tip] Tester sans token
> `apiinfo.version` répond **sans authentification** (et il ne faut pas envoyer de token). Pratique pour vérifier que l'URL est bonne.

---

## 📋 Les méthodes utilisées

| Méthode | Paramètres clés | Renvoie |
|---|---|---|
| `hostgroup.get` | `output`, `with_hosts: true` (seulement les groupes non vides) | Les groupes |
| `host.get` | `output: [hostid, name, status, description]`, `selectHostGroups` | Les hôtes et leurs groupes |
| `problem.get` | `recent: true`, `sortfield: [eventid]` | Les problèmes actifs **et** récemment résolus |
| `trigger.get` | `triggerids`, `selectHosts: [hostid]` | L'hôte de chaque trigger |

👉 Le détail de chaque appel (requête, réponse, champ par champ) est dans [[05 Zabbix — les méthodes utilisées]].

> [!warning] `problem.get` ne donne pas l'hôte
> Un problème ne contient que l'identifiant de son trigger (`objectid`). Pour savoir quelle machine est concernée, on fait un **second appel** `trigger.get` avec la liste des `objectid`, en **une seule fois** pour tous les problèmes.

---

## 🧱 Le client C#

Le cœur du client : un seul `POST` générique, réutilisé par toutes les méthodes.

```csharp
using HttpRequestMessage message = new HttpRequestMessage(HttpMethod.Post, this.BuildApiUri());
message.Headers.Authorization = new AuthenticationHeaderValue("Bearer", this._options.ApiToken);
message.Content = new StringContent(JsonSerializer.Serialize(request, _serializerOptions), Encoding.UTF8);
message.Content.Headers.ContentType = new MediaTypeHeaderValue("application/json-rpc");
```

| Code | Pourquoi |
|---|---|
| `BuildApiUri()` | L'URL configurée est celle du site Zabbix : on y ajoute `api_jsonrpc.php` |
| `AuthenticationHeaderValue("Bearer", …)` | Le token passe dans un header, jamais dans l'URL ni dans les logs |
| `"application/json-rpc"` | Le type de contenu attendu par Zabbix |

Les options de sérialisation :

```csharp
private static readonly JsonSerializerOptions _serializerOptions = new JsonSerializerOptions
{
    // Zabbix renvoie les nombres sous forme de chaînes ("status": "0").
    NumberHandling = JsonNumberHandling.AllowReadingFromString,
    DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
};
```

Et la conversion des erreurs : quelle que soit la panne, le reste de l'appli ne voit qu'une **`ZabbixException`**.

| Situation | Ce que fait le client |
|---|---|
| URL ou token absent | `ZabbixException` **sans aucun appel** |
| Réseau coupé (`HttpRequestException`) | `ZabbixException` |
| Pas de réponse avant le timeout | `ZabbixException` |
| Code HTTP autre que 2xx | `ZabbixException` avec le code |
| Réponse avec un champ `error` | `ZabbixException` avec le message de Zabbix |

---

## ⚠️ Les pièges de l'API

| Piège | Solution |
|---|---|
| Les nombres arrivent en texte (`"clock": "1790676802"`) | `JsonNumberHandling.AllowReadingFromString` |
| Une erreur revient avec un **HTTP 200** | Tester le champ `error` de la réponse |
| Les dates sont des secondes depuis 1970 | `DateTimeOffset.FromUnixTimeSeconds(clock).UtcDateTime` |
| Zabbix 7 utilise `selectHostGroups` | L'ancien `selectGroups` n'existe plus |
| Les identifiants sont des entiers 64 bits envoyés en texte | Les garder en `string` |
| Un paramètre « drapeau » (`with_hosts`, `recent`) compte **dès qu'il est présent** | Ne jamais l'envoyer à `false` : ne pas l'envoyer du tout |

---

## 🔗 Liens

- [[03 Zabbix — synchroniser dans sa base]] — ce qu'on fait de ces données
- [[00 API tierces]] — le projet-feuille qui contient ce client
- [[01 Tester sans dépendances externes]] — tester le client avec un faux serveur
