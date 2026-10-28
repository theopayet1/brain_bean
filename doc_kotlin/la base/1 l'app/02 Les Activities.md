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
maj: 2026-10-28
---

# Les Activities

> [!info] Définition
> **Kotlin (Android) fonctionne avec des activities.** Une activity est une **unité de l'application** : en gros, un **écran**.

Chaque activity est déclarée dans [[01 Le manifest|le manifest]], tourne dans un [[#Le contexte|contexte]] et traverse un [[#Le cycle de vie d'une Activity|cycle de vie]].

---

## Le contexte

> [!info] Définition
> Le **contexte** est la **classe de base d'Android** qui fournit les **contrats** permettant d'**accéder aux capacités matérielles du téléphone** (par exemple **vibrer**).

Une **activity est aussi un contexte** : il est **créé** à `onCreate` et **détruit** à sa mort. Les deux Contexts (Application et Activity), la fuite mémoire et où trouver un Context → [[01 Le Context]].

> [!warning] Lien avec l'injection de dépendances
> Une app au **purgatoire** (en arrière-plan) peut voir son **processus tué** par Android s'il manque de mémoire : toutes les activities et les dépendances injectées meurent avec. → voir [[01 Le manifest#Point d'attention injection de dépendances]]

---

## Le cycle de vie d'une Activity

Une activity passe par **4 états** reliés par des **méthodes de callback** : **mort** → `onCreate` → **purgatoire** → `onStart` → **visible** → `onResume` → **active**, puis on redescend par `onPause`, `onStop`, `onDestroy`. `onCreate` = **création du contexte**, `onDestroy` = **totalement mort**.

> [!tip] Tout le détail est dans sa propre note
> Les états, les schémas, quoi faire à quelle étape, la rotation et la version Compose → [[03 Le cycle de vie]].

---

## 🔗 Liens

- [[00 Les composants de base]]
- [[01 Le manifest]]
- [[04 MainActivity]] — créer sa MainActivity en pratique
