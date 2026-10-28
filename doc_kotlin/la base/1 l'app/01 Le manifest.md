---
tags:
  - projet/kotlin
  - type/concept
  - techno/kotlin
  - techno/android
  - sujet/manifest
  - sujet/permissions
  - statut/a-jour
cours: P1
cree: 2026-09-28
maj: 2026-10-28
---

# Le manifest

> [!info] Définition
> Le **manifest** est la **carte d'identité de l'application**.

C'est le fichier que le système Android lit en premier : il décrit **ce qu'est** l'application et **ce qu'elle contient** avant même qu'elle ne se lance.

---

## 🧩 À quoi il sert

```mermaid
flowchart TD
    M[📄 Manifest] --> N[Déclare le nom de l'app]
    M --> P["Déclare les permissions<br/>(ex. demande de vibration)"]
    M --> A[Déclare l'activity principale]
    M --> AP["Déclare la classe Application<br/>(là où on démarre Koin)"]
```

Le manifest **déclare l'app**, c'est-à-dire :

- le **nom de l'app** ;
- les **permissions** (exemple : **demande de vibration**) ;
- l'**[[02 Les Activities|activity principale]]** (le point d'entrée de l'app) ;
- la classe **`Application`** de l'app, s'il y en a une (`android:name`). C'est souvent là qu'on démarre l'**[[05 Koin (injection de dépendances)|injection de dépendances]]**.

> [!warning] Les libs ne sont pas dans le manifest
> Les **libs** (Compose, Koin, Ktor…) se déclarent dans **Gradle**, dans `app/build.gradle.kts` → [[02 Ajouter une dépendance]]. Le manifest, lui, déclare ce que l'app **est** et ce qu'elle a **le droit de faire**.

---

## Point d'attention injection de dépendances

> [!warning] Le purgatoire tue tout
> Si dans une activity on déclare des libs **avec injection de dépendances**, et que l'app est envoyée au **purgatoire**, alors **toutes les activities se font tuer**.

> [!note] Ce qui se passe vraiment (corrigé le 28 octobre 2026)
> Passer au purgatoire (`onStop`) ne détruit **pas** le contexte à lui seul. Mais une app au purgatoire, c'est une app **en arrière-plan** : si le téléphone manque de mémoire, Android peut **tuer tout le processus** de l'app (*process death*). Là, oui, **tout meurt d'un coup** : toutes les activities, les `single` de Koin, les ViewModels.
> Quand l'utilisateur revient, Android **recrée** l'activity et rappelle `Application.onCreate` : Koin redémarre et les objets injectés sont **fabriqués à neuf**. Ce qui n'était qu'en mémoire est **perdu** → ce qui doit survivre va dans Room ou `rememberSaveable` ([[03 Le cycle de vie]]).

L'outil d'injection utilisé est [[05 Koin (injection de dépendances)|Koin]]. L'autre piège avec l'injection, la **fuite mémoire**, est dans [[01 Le Context]].

---

## 🔐 Il déclare aussi les permissions

Le manifest **déclare** ce que l'app a le droit de faire (vibrer, aller sur internet, ouvrir la caméra…). Ce n'est **pas lui le gardien** : c'est **Android** qui, au moment d'agir, **vérifie le manifest**.

👉 Détail dans [[03 Les Permissions]].

---

## 🔗 Liens

- [[00 Les composants de base]]
- [[02 Les Activities]]
- [[03 Les Permissions]]
