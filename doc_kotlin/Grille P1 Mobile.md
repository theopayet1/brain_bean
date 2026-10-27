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
  - Checklist du rendu
cours: P1
cree: 2026-10-26
maj: 2026-10-26
---

# Grille d'évaluation P1 Mobile — où réviser

> [!abstract] En une phrase
> Les **16 critères** de la grille d'évaluation (P2028 DI2 P1 Mobile, créée le 14 octobre 2026), avec pour chacun **la note à relire**, puis la **checklist du rendu** pour viser « Maîtrisé ».

> [!info] Le barème
> Chaque critère est noté **Non réalisé (0)**, **Non acquis (5)**, **En cours d'acquisition (9)**, **Acquis (15)** ou **Maîtrisé (20)**. Pour les critères pratiques, le niveau dépend du **nombre de points « Maîtrisé »** que tu coches.

---

## 🧠 Théorie (ce que tu dois savoir expliquer)

| Code | Sujet | À relire |
|---|---|---|
| S-5 | **Fondamentaux** : Manifest, dossiers, Gradle, Activity, Context, **Intent**, cycle de vie | [[00 les composant de base]], [[03 Le cycle de vie]], [[06 Organisation du projet]], [[02 Les Intents]], [[01 Le Context]], [[00 Gradle]] |
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

Tous les points « Maîtrisé » des critères pratiques et qualité, regroupés. Coche au fur et à mesure.

### Manifest et Gradle

- [ ] Manifest **sans warning**, avec un **commentaire par permission** et par activity
- [ ] Gradle **sans warning** (attention à `kotlinOptions`), avec des **regions** ou des commentaires par famille de libs
- [ ] **Toutes** les versions dans `libs.versions.toml`, aucune écrite en dur, des noms clairs

### Ressources

- [ ] **Aucun texte en dur** dans les composables : tout passe par `stringResource`
- [ ] **Au moins une variante** de ressource : `values-en/` ou `values-night/`
- [ ] Les ressources suivent **une convention de nommage** (`ic_`, `écran_élément_rôle`…)

### Cycle de vie et extension

- [ ] **Une** utilisation **utile** du cycle de vie (pas « logger pour logger ») avec un **commentaire spécifique**
- [ ] **Une fonction d'extension sur `Context`**, avec un KDoc et des **block tags** (`@receiver`, `@param`, `@return`)

### Architecture

- [ ] Packages `ui/`, `domain/`, `data/`, `di/` ; Domain en **Kotlin pur**
- [ ] ViewModels qui dépendent des **interfaces** du Domain, branchées par **Koin**
- [ ] Couples **Service/DTO** et **DAO/Entity** séparés, **mappers** écrits, fetch **local / distant** cohérent

### Matériel

- [ ] **Au moins 2 capacités natives** (son, vibreur, GPS, gyroscope, base de données…)
- [ ] Chacune dans un **Manager**, **aucun** `getSystemService` dans un composable
- [ ] Les **permissions dangereuses** demandées à l'exécution

### UI

- [ ] Composables découpés en **Lego** (Screen → Content → composants de `ui/core/components`)
- [ ] **Un `UiState`** par écran, des **actions** de ViewModel aux noms clairs
- [ ] **Un composable complexe** avec un KDoc

### Qualité

- [ ] **Code → Inspect Code** sur tout le projet : plus de warning de nommage
- [ ] KDoc sur les fonctions et services de la couche Data, et sur les Managers
- [ ] **Une seule langue** pour les commentaires

---

## 🔗 Liens

- [[kotlin]] — accueil du cours
- [[Vérification des critères]] — chaque point « Maîtrisé » à cocher, avec le lien vers la section qui l'explique
- [[00 Architecture]] — la section qui pèse le plus dans la grille
