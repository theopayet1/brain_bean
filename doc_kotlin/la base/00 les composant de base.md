---
tags:
  - projet/kotlin
  - type/index
  - techno/kotlin
  - techno/android
  - sujet/apk
  - statut/a-jour
cours: P1
cree: 2026-09-28
maj: 2026-10-26
---

# Les composants de base d'une application Android

> [!abstract] En une phrase
> Une **application** Android est livrée sous forme d'**APK**. À l'intérieur de cet APK on trouve le **manifest**, les **activities** (+ leur contexte), les **ressources** et le **`classes.dex`** (le code compilé).

---

## 📦 À l'intérieur d'un APK (app)

```mermaid
flowchart TD
    APK[📦 APK = l'application]
    APK --> M["📄 Manifest<br/>carte d'identité"]
    APK --> A["🧩 Activity<br/>écran + contexte"]
    APK --> R["📁 Ressources (res)<br/>non compilées"]
    APK --> D["⚙️ classes.dex<br/>code compilé = exécutable"]
```

| Composant | Rôle (résumé) | Note détaillée |
|---|---|---|
| **Manifest** | Carte d'identité : **nom de l'app**, **permissions** (ex. vibration), **activity principale** | [[01 Le manifest]] |
| **Activity** | Un **écran** ; fournit aussi un **contexte** (accès aux capacités matérielles) | [[02 Les Activities]] |
| **Ressources (`res`)** | **Non compilées** : images, sons, gifs, couleurs, strings (multilingue) | [[04 Les Ressources]] |
| **`classes.dex`** | Le **code compilé**, la **partie exécutable** de l'app | [[07 Classes.dex]] |

---

## 🧱 Autres notions

| Notion | Rôle | Note |
|---|---|---|
| **Cycle de vie** | Les états d'une activity, quoi faire à quelle étape, la rotation | [[03 Le cycle de vie]] |
| **Architecture** | Screen / ViewModel / Model | [[05 Architecture (Screen - ViewModel - Model)]] |
| **Permissions** | Ce que l'app a le droit de faire | [[06 Les Permissions]] |
| **UI déclarative** | L'UI décrite en déclaratif (plus du « compilé ») | [[08 UI déclarative]] |
| **Koin / DI** | Fournir les objets de l'extérieur (injection) | [[09 Koin (injection de dépendances)]] |
| **Organisation du projet** | Les dossiers et les packages par couche | [[10 Organisation du projet]] |
| **Intent** | Le message « je veux faire ça » envoyé à Android | [[11 Les Intents]] |
| **Context** | Le guichet vers le système (Application vs Activity) | [[12 Le Context]] |
| **Capacités natives** | Vibreur, capteurs, son… rangés dans des Managers | [[13 Les capacités natives (Managers)]] |
| **Ressources en pratique** | `stringResource`, variantes, nommage | [[14 Les ressources en pratique]] |

---

## 🔗 Comment ça s'articule

```mermaid
flowchart LR
    M[📄 Manifest] -->|déclare| A[🧩 Activity]
    A -->|fournit| C[📱 Contexte<br/>capacités matérielles]
    A -->|utilise| R[📁 Ressources]
    D[⚙️ classes.dex] -->|exécute| A
    M -->|autorise| P[🔐 Permissions]
```

---

## ➡️ Suite

- [[01 Le manifest]]
- [[02 Les Activities]]
- [[03 Le cycle de vie]]
- [[04 Les Ressources]]
- [[05 Architecture (Screen - ViewModel - Model)]]
- [[06 Les Permissions]]
- [[07 Classes.dex]]
- [[08 UI déclarative]]
- [[09 Koin (injection de dépendances)]]
- [[10 Organisation du projet]]
- [[11 Les Intents]]
- [[12 Le Context]]
- [[13 Les capacités natives (Managers)]]
- [[14 Les ressources en pratique]]
