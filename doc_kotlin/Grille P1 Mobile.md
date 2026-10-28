---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - sujet/evaluation
  - statut/a-jour
aliases:
  - Grille d'évaluation P1 Mobile
cours: P1
cree: 2026-10-26
maj: 2026-10-28
---

# Grille d'évaluation P1 Mobile — où réviser

> [!abstract] En une phrase
> Les **16 critères** de la grille d'évaluation (P2028 DI2 P1 Mobile, créée le 14 octobre 2026), avec pour chacun **la note à relire**, et le lien vers la **checklist du rendu** pour viser « Maîtrisé ».

> [!info] Le barème
> Chaque critère est noté **Non réalisé (0)**, **Non acquis (5)**, **En cours d'acquisition (9)**, **Acquis (15)** ou **Maîtrisé (20)**. Pour les critères pratiques, le niveau dépend du **nombre de points « Maîtrisé »** que tu coches.

---

## 🧠 Théorie (ce que tu dois savoir expliquer)

| Code | Sujet | À relire |
|---|---|---|
| S-5 | **Fondamentaux** : Manifest, dossiers, Gradle, Activity, Context, **Intent**, cycle de vie | [[00 Les composants de base]], [[03 Le cycle de vie]], [[06 Organisation du projet]], [[02 Les Intents]], [[01 Le Context]], [[00 Gradle]] |
| S-1 | **Clean Architecture** : rôle des couches, dépendances vers l'intérieur | [[00 Architecture]] |
| S-3 | Couche **Domain** : Domain Models, interfaces Repository | [[02 La couche Domain]] |
| S-2 | Couche **Data** : fetch, Service/DTO, DAO/Entity, mapping | [[03 La couche Data]] |
| S-4 | Couche **UI** : Compose, **MVI**, **UDF**, états, Single Activity, navigation | [[04 La couche UI (MVI et UDF)]], [[00 Compose]], [[04 MainActivity]] |
| S-6 | **Capacités natives** : rôle du Context, **Managers**, UI sans logique matérielle | [[04 Les capacités natives (Managers)]], [[03 Les Permissions]] |

> [!tip] À l'oral
> Pour « Maîtrisé », la grille répète : explications **claires**, qui vont **droit au but**, et capacité à **schématiser**. Entraîne-toi à redessiner les schémas des notes sans les regarder.

---

## 🛠️ Pratique (ce que ton code doit montrer)

| Code | Sujet | À relire |
|---|---|---|
| SF-5 | **Fondamentaux** dans le projet : manifest, Gradle, ressources, cycle de vie, extension | [[4.1 Les ressources en pratique]], [[03 Le cycle de vie]], [[04 Les fonctions d'extension]] |
| SF-1 | **Architecture en couches** + Koin | [[00 Architecture]], [[05 Koin (injection de dépendances)]] |
| SF-3 | Composants **Domain** indépendants | [[02 La couche Domain]] |
| SF-4 | Composants **Data** : DAO/Entity, Service/DTO, fetch, mapping | [[03 La couche Data]] |
| SF-6 | **Matériel** : 2 capacités natives, Managers, extension sur Context | [[04 Les capacités natives (Managers)]], [[4.1 Le vibreur pas à pas]], [[4.2 La localisation pas à pas]] |

## ✨ Qualité du code (expérience du développeur)

| Code | Sujet | À relire |
|---|---|---|
| SE-1 | Couche **Data** : KDoc, découpage, pas de duplication, noms explicites | [[03 La couche Data]], [[05 KDoc et commentaires]] |
| SE-2 | Couche **UI** : Lego, noms, actions du VM, State, KDoc d'un composable | [[04 La couche UI (MVI et UDF)]] |
| SE-3 | Couche **Domain** : headers KDoc sur la logique métier | [[02 La couche Domain]] |
| SE-4 | **Projet** : manifest et Gradle commentés, versions, ressources, cycle de vie, extension | [[05 KDoc et commentaires]], [[06 Conventions de nommage et Lint]] |
| SE-5 | **Qualité** : Managers, conventions Kotlin (Lint), KDoc de l'extension du Context | [[06 Conventions de nommage et Lint]], [[04 Les fonctions d'extension]] |

---

## ✅ Checklist du rendu

> [!tip] Elle a sa propre note
> Tous les points « Maîtrisé » à cocher, critère par critère, avec le lien vers la note qui explique chaque point → [[Vérification des critères]].

---

## 🔗 Liens

- [[kotlin]] — accueil du cours
- [[Vérification des critères]] — chaque point « Maîtrisé » à cocher, avec le lien vers la section qui l'explique
- [[00 Architecture]] — la section qui pèse le plus dans la grille
