---
tags:
  - projet/kotlin
  - type/index
  - techno/kotlin
  - techno/android
  - statut/a-jour
aliases:
  - Kotlin / Android — MOC
  - Accueil Kotlin
cours: P1
cree: 2026-09-28
maj: 2026-10-26
---

# 🗂️ Kotlin / Android — MOC

> [!abstract] Map of Content
> Page d'accueil du cours **Kotlin / Android (P1)**. Point d'entrée vers toutes les notes.

## 🎯 Révision

- [[Grille P1 Mobile]] — les 16 critères de la grille, la note à relire pour chacun, et la checklist du rendu

## 🧱 La base

- [[00 les composant de base]] — vue d'ensemble des composants
- [[01 Le manifest]] — la carte d'identité de l'app
- [[02 Les Activities]] — activities & contexte
- [[03 Le cycle de vie]] — les 4 états, quoi faire à quelle étape, la rotation, la version Compose
- [[04 Les Ressources]] — le répertoire `res` & la reconfiguration
- [[05 Architecture (Screen - ViewModel - Model)]] — Screen / ViewModel / Model
- [[06 Les Permissions]] — Android vérifie le manifest & les autorisations
- [[07 Classes.dex]] — le code compilé / exécutable
- [[08 UI déclarative]] — l'UI en déclaratif (Compose)
- [[09 Koin (injection de dépendances)]] — fournir les objets de l'extérieur
- [[10 Organisation du projet]] — les dossiers et les packages par couche
- [[11 Les Intents]] — le message « je veux faire ça » envoyé à Android
- [[12 Le Context]] — le guichet vers le système, Application vs Activity
- [[13 Les capacités natives (Managers)]] — vibreur, capteurs, son… rangés dans des Managers
- [[14 Les ressources en pratique]] — `stringResource`, variantes (langue, nuit), nommage

## 🎨 Compose

- [[00 Compose]] — fonctions, philosophie Kotlin, métaphore Lego
- [[01 Les composants]] — card, row, column, image, text…
- [[02 Le modifier]] — configurer / décorer un composant
- [[03 MainActivity]] — construire `MainActivity.kt` étape par étape (Scaffold, NavHost, navbar)
- [[04 Les previews]] — `@Preview` : voir un composable sans lancer l'émulateur

## 💻 Code (mémo Kotlin)

- [[00 Code]] — index des sous-catégories
- [[01 Kotlin vs Java]] — pas d'accesseurs à coder, etc.
- [[02 Les classes]] — constructeur, membre vs paramètre
- [[03 Les logs]] — écrire/lire des logs, niveaux, bonnes pratiques
- [[04 Les notifications push]] — FCM, le token et la liaison avec un backend .NET
  - [[4.1 Notifications push côté Kotlin]] — le code Android pas à pas
  - [[4.2 Notifications push côté .NET]] — le code backend pas à pas
- [[05 Les fonctions d'extension]] — ajouter une fonction à `Context`, `Intent`…
- [[06 KDoc et commentaires]] — headers KDoc, block tags, manifest et Gradle commentés
- [[07 Conventions de nommage et Lint]] — nommer en Kotlin, chasser les warnings

## 🐘 Gradle

- [[00 Gradle]] — ce que fait Gradle, le réflexe « Sync Now »
- [[01 Les fichiers Gradle]] — la carte des fichiers `.gradle.kts` et du catalogue
- [[02 Ajouter une dépendance]] — ajouter une lib pas à pas (catalogue, BOM)
- [[03 Le bloc android]] — `namespace`, `minSdk`, `targetSdk`… ligne par ligne
- [[04 Commandes et erreurs Gradle]] — `gradlew` et les erreurs fréquentes

## 🏛️ Architecture

- [[00 Architecture]] — la Clean Architecture : UI, Domain, Data et la règle de dépendance
- [[01 La couche Domain]] — modèles, interfaces Repository, use cases
- [[02 La couche Data]] — Service/DTO, DAO/Entity, mapping, mécanisme de fetch
- [[03 La couche UI (MVI et UDF)]] — état, actions, flux unidirectionnel, Single Activity

## 🧭 Parcours conseillé

```mermaid
flowchart TD
    I[00 Composants de base] --> M[01 Le manifest]
    M --> A[02 Les Activities]
    A --> CV[03 Le cycle de vie]
    CV --> R[04 Les Ressources]
    R --> D[07 Classes.dex]
    D --> G[🐘 Gradle]
    G --> AR[05 Architecture]
    AR --> U[08 UI déclarative]
    AR --> P[06 Les Permissions]
    U --> CP[🎨 Compose]
    I --> O[10 Organisation du projet]
    A --> IN[11 Les Intents]
    A --> CX[12 Le Context]
    R --> RP[14 Ressources en pratique]
    CX --> MG[13 Capacités natives]
    P --> MG
    AR --> ARC[🏛️ Architecture]
    CP --> ARC
```

## 🏷️ Tags du cours

Chaque note a au moins `#projet/kotlin`, un `#type/…` et un `#statut/…`.

| Famille | Tags utilisés ici |
|---|---|
| `techno/` | `#techno/kotlin` `#techno/android` `#techno/compose` `#techno/koin` `#techno/gradle` `#techno/java` `#techno/dotnet` `#techno/csharp` `#techno/efcore` |
| `sujet/` | `#sujet/apk` `#sujet/manifest` `#sujet/activity` `#sujet/cycle-de-vie` `#sujet/ressources` `#sujet/compilation` `#sujet/architecture` `#sujet/mvvm` `#sujet/ui` `#sujet/permissions` `#sujet/securite` `#sujet/injection-de-dependances` `#sujet/navigation` `#sujet/debug` `#sujet/build` `#sujet/dependances` `#sujet/notifications` `#sujet/clean-architecture` `#sujet/domain` `#sujet/data` `#sujet/mvi` `#sujet/etat` `#sujet/intent` `#sujet/context` `#sujet/capacites-natives` `#sujet/extension` `#sujet/documentation` `#sujet/conventions` `#sujet/qualite` `#sujet/evaluation` |
| `type/` | `#type/index` `#type/concept` `#type/guide` `#type/reference` |
| `statut/` | `#statut/a-jour` `#statut/brouillon` |
