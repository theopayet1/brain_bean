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
maj: 2026-10-01
---

# Zabbix — pièges et limites

> [!abstract] En une phrase
> Ce qui peut surprendre quand on branche une API sur Zabbix : des alertes qui n'en sont pas, des réponses vides sans erreur, et quelques erreurs classiques avec leur solution. À lire avant : [[03 Zabbix — synchroniser dans sa base]].

---

## 🚨 Les faux positifs

Avec les templates par défaut, la découverte automatique des disques surveille aussi des **pseudo-systèmes de fichiers** Linux (`/dev/hugepages`, `/proc/sys/fs/binfmt_misc`…). Ils affichent toujours **0 % d'espace libre**, donc leur trigger « espace disque critique » est **déclenché en permanence**.

> [!warning] Des dizaines d'alertes *High* qui n'en sont pas
> Une quinzaine de règles de ce type par machine Linux, multipliées par le nombre de machines, peuvent noyer les vraies alertes.

> [!success] À corriger dans Zabbix, pas dans l'API
> Exclure ces systèmes de fichiers dans les **filtres de la règle de découverte** des disques (les macros `{$VFS.FS.FSNAME.NOT_MATCHES}` / `{$VFS.FS.FSTYPE.NOT_MATCHES}` du template). Filtrer côté API cacherait le problème sans le régler.

---

## 🕳️ Une réponse vide n'est pas une erreur

Si l'utilisateur du token n'a pas le droit de lire un groupe d'hôtes, Zabbix ne renvoie **pas d'erreur** : il renvoie une **liste vide**.

> [!warning] Conséquence sur la synchro
> Si le token perd ses droits, `host.get` renvoie une liste vide : la synchro **désactive tous les hôtes** et **marque toutes les alertes résolues**. Tout revient à la synchro suivante une fois les droits rétablis, mais c'est brutal.

---

## 🧯 Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| `Not authorised` dans l'erreur Zabbix | Token faux, expiré, ou utilisateur sans droits | Vérifier le token et les **permissions de lecture** de l'utilisateur API sur les groupes d'hôtes |
| Réponse vide alors que Zabbix contient des données | L'utilisateur API ne voit pas ces groupes | Donner les droits de lecture sur les groupes concernés |
| `Invalid parameter … unexpected parameter "selectGroups"` | Paramètre de Zabbix ≤ 6.0 | Utiliser `selectHostGroups` (Zabbix 6.2 et plus) |
| Warning « synchronization failed » toutes les minutes | Zabbix injoignable (réseau, URL) | Tester `apiinfo.version` avec `curl`. L'API continue de tourner et réessaie seule |
| Warning « Zabbix:Url or Zabbix:ApiToken is not configured » au démarrage | Token absent du `.env` ou des user-secrets | Le renseigner, puis redémarrer l'API |
| Une modification faite par l'API disparaît une minute plus tard | La synchro écrase ce qui vient de Zabbix | Tant que l'écriture vers Zabbix n'existe pas, ne pas modifier à la main les données importées |
| Faux hôtes (données de démo) mélangés aux vrais | La base a été remplie par le seed avant la synchro | Mettre `Seed:SampleData` à `false`, puis repartir d'une base propre |
| Deux instances de l'API → erreurs d'index unique dans les logs | Deux synchros écrivent dans la même base en même temps | N'en lancer qu'une (arrêter le conteneur quand on débogue dans l'IDE) |
| Swagger : « Failed to fetch » | L'API n'est pas démarrée (la page Swagger est restée ouverte) | Relancer l'API, puis recharger la page |

---

> [!todo] À compléter
> - [ ] Écrire dans Zabbix depuis l'API (acquitter un problème, modifier un hôte)
> - [ ] Protéger la synchro contre une réponse vide due à un problème de droits

---

## 🔗 Liens

- [[01 Zabbix — les concepts]] — items, triggers, templates
- [[05 Zabbix — les méthodes utilisées]] — les appels et les droits nécessaires
- [[01 Rider — lancer l'API en local ou dans Docker]] — éviter deux API en même temps
