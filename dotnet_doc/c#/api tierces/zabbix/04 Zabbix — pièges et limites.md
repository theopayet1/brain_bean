---
tags:
  - projet/csharp
  - type/reference
  - techno/zabbix
  - techno/docker
  - sujet/api-tierce
  - sujet/supervision
  - sujet/debug
  - statut/a-jour
aliases:
  - Zabbix — erreurs fréquentes
cree: 2026-09-30
maj: 2026-09-30
---

# Zabbix — pièges et limites

> [!abstract] En une phrase
> Ce qui peut surprendre quand on branche une API sur Zabbix : des alertes qui n'en sont pas, des métriques qui manquent, et quelques erreurs classiques avec leur solution. À lire avant : [[03 Zabbix — synchroniser dans sa base]].

---

## 🚨 Les faux positifs

Avec les templates par défaut, la découverte automatique des disques surveille aussi des **pseudo-systèmes de fichiers** Linux (`/dev/hugepages`, `/proc/sys/fs/binfmt_misc`…). Ils affichent toujours **0 % d'espace libre**, donc leur trigger « espace disque critique » est **déclenché en permanence**.

> [!warning] Des dizaines d'alertes *High* qui n'en sont pas
> Une quinzaine de règles de ce type par machine Linux, multipliées par le nombre de machines, peuvent noyer les vraies alertes.

> [!success] À corriger dans Zabbix, pas dans l'API
> Exclure ces systèmes de fichiers dans les **filtres de la règle de découverte** des disques (les macros `{$VFS.FS.FSNAME.NOT_MATCHES}` / `{$VFS.FS.FSTYPE.NOT_MATCHES}` du template). Filtrer côté API cacherait le problème sans le régler.

---

## 📉 Les métriques incomplètes

| Symptôme | Cause |
|---|---|
| Un hôte n'a jamais de mesure CPU / RAM / disque | Aucune des clés candidates n'existe sur cet hôte : son template utilise d'autres clés |
| Un item existe mais sa valeur vaut 0 | `lastclock = 0` : l'item n'a **jamais été collecté** (agent injoignable, item mal configuré) |
| Un hôte Windows n'a presque aucun item | L'agent n'est pas joignable, ou aucun template n'est appliqué |

**Pistes :**
- corriger les templates ou l'agent dans Zabbix ;
- ajouter la bonne clé dans `Zabbix:MetricKeys`, sans toucher au code ;
- accepter des valeurs **facultatives** dans `HostState` (`int?`), mais cela change le contrat de l'API pour le front.

Pour voir les clés réellement collectées sur un hôte :

```bash
curl -s -X POST https://zabbix.example.com/api_jsonrpc.php \
  -H "Content-Type: application/json-rpc" -H "Authorization: Bearer $ZABBIX_API_TOKEN" \
  -d '{"jsonrpc":"2.0","method":"item.get","params":{"output":["key_","lastvalue","lastclock","state"],"host":"srv-web-01","search":{"key_":["cpu","memory","vfs.fs"]},"searchByAny":true},"id":1}'
```

---

## 🙋 Aucun incident

La table des incidents reste vide tant que **personne n'acquitte de problème** dans Zabbix. Ce n'est pas un bug : pour tester, acquitter un problème dans l'interface Zabbix (**Monitoring → Problems → Update**) et attendre la synchro suivante.

---

## 🧯 Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| `Not authorised` dans l'erreur Zabbix | Token faux, expiré, ou utilisateur sans droits sur les groupes | Vérifier le token et les **permissions de lecture** de l'utilisateur API sur les groupes d'hôtes |
| Réponse vide alors que Zabbix contient des données | L'utilisateur API ne voit pas ces groupes | Donner les droits de lecture sur les groupes concernés |
| `Invalid parameter … unexpected parameter "selectGroups"` | Paramètre de Zabbix ≤ 6.0 | Utiliser `selectHostGroups` (Zabbix 6.2 et plus) |
| Warning « synchronization failed » toutes les minutes | Zabbix injoignable (réseau, URL) | Tester `apiinfo.version` avec `curl`. L'API continue de tourner et réessaie seule |
| Warning « Zabbix:Url or Zabbix:ApiToken is not configured » au démarrage | Token absent du `.env` ou des user-secrets | Le renseigner, puis redémarrer l'API |
| Des alertes créées à la main disparaissent ou changent | La synchro écrase ce qui vient de Zabbix | Tant que les modifications vers Zabbix n'existent pas, ne pas modifier à la main les données importées |
| Deux instances de l'API → erreurs d'index unique dans les logs | Deux synchros écrivent dans la même base en même temps | N'en lancer qu'une (arrêter le conteneur quand on débogue dans l'IDE) |
| Swagger : « Failed to fetch » | L'API n'est pas démarrée (la page Swagger est restée ouverte) | Relancer l'API, puis recharger la page |

---

> [!todo] À compléter
> - [ ] Modifier des données dans Zabbix depuis l'API (acquitter, désactiver un trigger)
> - [ ] Décider pour les `HostState` incomplets

---

## 🔗 Liens

- [[01 Zabbix — les concepts]] — items, triggers, templates
- [[02 Zabbix — l'API JSON-RPC]] — les méthodes et les pièges de l'API
- [[01 Rider — lancer l'API en local ou dans Docker]] — éviter deux API en même temps
