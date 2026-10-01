---
tags:
  - projet/csharp
  - type/index
  - techno/zabbix
  - techno/dotnet
  - sujet/api-tierce
  - sujet/supervision
  - statut/a-jour
aliases:
  - CSharp — Zabbix
cree: 2026-09-30
maj: 2026-10-01
---

# Zabbix

> [!abstract] En une phrase
> **Zabbix** est un outil de **supervision** : il surveille des machines (CPU, RAM, disques, services…) et ouvre un **problème** quand quelque chose dépasse un seuil. Cette section explique comment une API .NET récupère ses données : ses concepts, son API JSON-RPC, et la synchronisation dans notre propre base.

---

## 🧩 Ce qu'on en récupère

| Donnée Zabbix | Pour quoi faire |
|---|---|
| Les **groupes d'hôtes** | Ranger les machines |
| Les **hôtes** | La liste des machines surveillées, actives ou désactivées |
| Les **problèmes** | Les alertes en cours ou résolues, avec leur sévérité et leur machine |

> [!note] Pas encore récupéré
> Les **acquittements** (le suivi humain des alertes) et les **mesures** CPU, RAM, disque restent dans Zabbix pour l'instant.

> [!info] Version
> Tout est écrit pour **Zabbix 7.0**. L'authentification par token dans le header `Authorization: Bearer` existe depuis la **6.4**.

---

## 📚 Notes de la section

| Note | Contenu |
|---|---|
| [[01 Zabbix — les concepts]] | Groupe, hôte, item, trigger, problème, acquittement, sévérité |
| [[02 Zabbix — l'API JSON-RPC]] | S'authentifier, appeler les méthodes, le client C#, les pièges |
| [[03 Zabbix — synchroniser dans sa base]] | Le choix d'architecture, la correspondance des données, l'ordre de la synchro |
| [[04 Zabbix — pièges et limites]] | Faux positifs, réponses vides, erreurs fréquentes |
| [[05 Zabbix — les méthodes utilisées]] | La liste des méthodes de l'API appelées, avec leurs paramètres et leurs réponses |

---

## 🔗 Liens

- [[00 API tierces]] — les règles communes à toutes les API externes
- [[csharp]] — accueil du vault
