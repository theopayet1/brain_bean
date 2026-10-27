---
tags:
  - projet/kotlin
  - type/concept
  - techno/kotlin
  - techno/android
  - sujet/activity
  - sujet/cycle-de-vie
  - statut/a-jour
cours: P1
cree: 2026-09-28
maj: 2026-10-26
---

# Les Activities

> [!info] Définition
> **Kotlin (Android) fonctionne avec des activities.** Une activity est une **unité de l'application** : en gros, un **écran**.

Chaque activity est déclarée dans [[01 Le manifest|le manifest]], tourne dans un [[#Le contexte|contexte]] et traverse un [[#Le cycle de vie d'une Activity|cycle de vie]].

---

## Le contexte

> [!info] Définition
> Le **contexte** est la **classe de base d'Android** qui fournit les **contrats** permettant d'**accéder aux capacités matérielles du téléphone** (par exemple **vibrer**).

Une **activity est aussi un contexte**. C'est par lui qu'on accède aux ressources et capacités du système (vibration, notifications, fichiers, services…). Le contexte est **créé** au lancement de l'activity (étape `onCreate`) et **détruit** à sa mort.

> [!warning] Lien avec l'injection de dépendances
> Comme les dépendances injectées reposent sur le contexte, le passage au **purgatoire** qui détruit le contexte fait **tuer les activities**. → voir [[01 Le manifest#Point d'attention injection de dépendances]]

---

## Le cycle de vie d'une Activity

Une activity passe par **4 états** reliés par des **méthodes de callback** : **mort** → `onCreate` → **purgatoire** → `onStart` → **visible** → `onResume` → **active**, puis on redescend par `onPause`, `onStop`, `onDestroy`. `onCreate` = **création du contexte**, `onDestroy` = **totalement mort**.

> [!tip] Tout le détail est dans sa propre note
> Les états, les schémas, quoi faire à quelle étape, la rotation et la version Compose → [[03 Le cycle de vie]].

---

## 🔗 Liens

- [[00 les composant de base]]
- [[01 Le manifest]]
- [[03 MainActivity]] — créer sa MainActivity en pratique
