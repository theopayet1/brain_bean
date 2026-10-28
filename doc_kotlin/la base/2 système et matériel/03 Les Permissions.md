---
tags:
  - projet/kotlin
  - type/concept
  - techno/kotlin
  - techno/android
  - sujet/permissions
  - sujet/securite
  - statut/a-jour
cours: P1
cree: 2026-09-28
maj: 2026-10-28
---

# Les Permissions

> [!abstract] Idée clé
> Pour qu'un composant ait le droit de **faire quelque chose** (vibrer, aller sur internet, ouvrir la caméra…), il faut que ce soit **déclaré dans le [[01 Le manifest|manifest]]**. C'est **Android** (le gardien) qui **vérifie le manifest** et bloque si ce n'est pas déclaré.
>
> ⚠️ Le manifest n'est **pas** le gardien : c'est la **carte d'identité** qui *déclare*. Le **gardien, c'est le système Android**.

---

## 🪧 La métaphore : post-it & punaise

> [!quote] Tes mots
> Le **contexte vit dans l'application** ; on peut dire que l'**app est un post-it**.
> Mais la **classe `.dex`**, elle, est une **punaise** : elle **passe à travers** et **reste en vie** du coup.

```mermaid
flowchart TD
    APP[🟨 App = post-it] --- CTX[Le contexte vit dedans]
    DEX[📌 classe .dex = punaise] -->|passe à travers<br/>et reste en vie| APP
```

> [!note] Rappel
> Techniquement, **`classes.dex`** est le **code compilé / la partie exécutable** de l'app (voir [[05 Classes.dex]]). La « punaise » est la **métaphore de cours** pour dire qu'il s'exécute et veut accéder aux capacités du tél.

---

## 🚪 Le gardien (Android) : déclaré ou bloqué

> [!quote] Tes mots
> Quand la **classe `.dex` veut sortir** — exemple **faire vibrer** — *hop*, il y a un **gardien**. S'il **n'est pas déclaré dans le manifest**, alors *hop* **on le détruit**, rien à faire.

👉 Le **gardien, c'est Android** : il **regarde le manifest**. Ce qui n'y est **pas déclaré** n'a pas le droit de sortir.

```mermaid
flowchart LR
    A[Composant veut agir<br/>ex. faire vibrer] --> G{🚪 Android vérifie :<br/>déclaré dans le manifest ?}
    G -->|oui| OK[✅ autorisé]
    G -->|non| KO[❌ bloqué]
```

---

## 🦠 Deux types de permissions

### 1. Permissions « simples » (ex. vibrer)

> [!quote] Tes mots
> C'est un peu comme le **covid** : on doit **demander autorisation**, comme pour vibrer… alors qu'**il n'y a pas de popup** pour vibrer. « Alors c'est con. »

> [!note] Corrigé le 28 octobre 2026
> Ton exemple de cours était « **jouer un son** ». En fait, jouer un son ne demande **aucune** permission. Le bon exemple de permission simple (le vrai mot : permission **normale**), c'est **`VIBRATE`** ou **`INTERNET`** : déclarées dans le manifest, accordées à l'installation, sans popup.

➡️ Déclarées dans le manifest, mais **sans popup** à l'utilisateur.

### 2. Permissions **invasives** (caméra, micro, localisation…)

> [!quote] Tes mots
> Il y a aussi celles qui sont **invasives** comme **caméra, micro, localisation** etc.
> Exemple : l'app veut ouvrir la caméra → une sorte d'**alarme** se déclenche. Cette fois c'est une **demande d'autorisation**.

Déroulé de la demande :

```mermaid
flowchart TD
    R[📸 App veut la caméra] --> AL[🚨 Demande d'autorisation émise]
    AL --> CHK{Le tél vérifie<br/>les réglages :<br/>déjà autorisé ?}
    CHK -->|oui| OK[✅ accès accordé]
    CHK -->|non| ASK[❓ demande à l'utilisateur]
```

- Une **demande d'autorisation** est émise.
- Le **téléphone vérifie dans les réglages** si l'utilisateur l'a **déjà autorisé ou non**.
- **Si non** → il **demande à l'utilisateur**.

---

## ⛓️ Pas déclaré = bloqué

> [!quote] Tes mots
> Pas déclaré → « **prison android le pète, il en a rien à battre** ».

➡️ Si ce n'est **pas déclaré dans le manifest**, Android **ne laisse pas faire** (composant bloqué / tué).

---

## 🔗 Liens

- [[01 Le manifest]] — où se déclarent les permissions
- [[03 Le cycle de vie]] — la destruction des composants
- [[00 Les composants de base]]
