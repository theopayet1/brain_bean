---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - sujet/evaluation
  - statut/a-jour
aliases:
  - Fiche de révision globale
  - Résumé du cours
cours: P1
cree: 2026-10-28
maj: 2026-10-28
---

# Fiche de révision

> [!abstract] En une phrase
> **Tout le cours en une page** : une ou deux phrases par notion, et le lien vers la note si tu veux creuser. Tes points faibles sont détaillés dans [[Fiche points faibles]].

---

## 📦 L'app Android

| Notion | À retenir |
|---|---|
| **APK** | L'app livrée : manifest + code compilé (`classes.dex`) + ressources (`res`) → [[00 Les composants de base]] |
| **Manifest** | La **carte d'identité** : nom, icône, **permissions**, **activities** (dont la principale, `MAIN` / `LAUNCHER`). C'est **Android** le gardien qui le lit → [[01 Le manifest]] |
| **Activity** | Un **écran**, qui est aussi un **Context** → [[02 Les Activities]] |
| **Ressources** | `res/` : textes, couleurs, images, sons. **Jamais de texte en dur** (`stringResource`). Variantes : `values-en/`, `values-night/` → [[04 Les Ressources]], [[4.1 Les ressources en pratique]] |
| **`classes.dex`** | Le **code compilé**, la partie exécutable → [[05 Classes.dex]] |
| **Gradle** | Le **chef de chantier** : récupère les libs, compile, assemble l'APK. Versions dans **`libs.versions.toml`**. Réflexe : **Sync Now** → [[00 Gradle]], [[01 Les fichiers Gradle]] |

---

## 🔄 Le cycle de vie

```
mort ⇄ purgatoire ⇄ visible ⇄ active
   onCreate/onDestroy   onStart/onStop   onResume/onPause
```

- **Visible** = affichée mais pas au premier plan. **Purgatoire** = arrière-plan.
- **Règle des paires** : ce qu'on allume dans `onResume`, on l'éteint dans `onPause`.
- **Rotation** = l'activity est **détruite puis recréée**. Seuls le **ViewModel** et `rememberSaveable` survivent.

→ [[03 Le cycle de vie]]

---

## 🛎️ Le système

| Notion | À retenir |
|---|---|
| **Context** | Le **médiateur** entre ton code et le système : `getSystemService`, `getString`, `startActivity` → [[01 Le Context]] |
| **Deux Contexts** | **Application** (vit tant que l'app tourne : Koin, Managers) et **Activity** (de `onCreate` à `onDestroy` : écran, popup). **Jamais** d'Activity Context dans un objet qui vit plus longtemps → **fuite mémoire** |
| **Intent** | Un message à Android : « je veux faire ça ». **Explicite** = on dit **qui**, **implicite** = on dit **quoi**. `PendingIntent` = préparé pour plus tard (notifs) → [[02 Les Intents]] |
| **Permissions** | **Normales** (pas de popup, ex. `VIBRATE`, `INTERNET`) ou **dangereuses** (popup, ex. caméra, GPS). Toujours **déclarées** dans le manifest → [[03 Les Permissions]] |
| **Managers** | **Une classe par capacité** (vibreur, GPS, capteur) : le seul endroit qui touche au matériel → [[04 Les capacités natives (Managers)]] |

---

## 🏛️ La Clean Architecture

```
UI  ──►  Domain  ◄──  Data
```

| Couche | Rôle | Contenu |
|---|---|---|
| **UI** | **Afficher** | Screens, ViewModels, `UiState` |
| **Domain** | **Le métier**, en Kotlin pur, ne dépend de **rien** | Modèles, **interfaces** Repository, use cases |
| **Data** | **Aller chercher** les données | Service/DTO (API), DAO/Entity (Room), mappers, Managers |

- **Pourquoi** : testable sans émulateur, indépendant des frameworks, maintenable, travail en équipe.
- **Comment Data dépend du Domain** : le Domain **déclare** l'interface, Data l'**implémente**, **Koin** branche les deux.
- **Mapping** : `HostDto → Host`, `HostEntity → Host`. Si l'API change, seul Data bouge.
- **Fetch offline-first** : l'écran lit **toujours Room**, l'API **rafraîchit Room**.

→ [[00 Architecture]], [[02 La couche Domain]], [[03 La couche Data]], [[05 Koin (injection de dépendances)]]

---

## 🎨 L'UI avec Compose

| Notion | À retenir |
|---|---|
| **Déclaratif** | On **décrit** l'écran selon l'état ; quand l'état change, Compose **recompose** → [[01 UI déclarative]] |
| **UDF** | **L'état descend, les actions remontent.** L'écran ne modifie jamais les données lui-même |
| **MVI** | Model (l'état), View (l'affichage), Intent (l'**intention de l'utilisateur**, pas l'Intent Android) |
| **UiState** | **Un seul** par écran, une `data class` immuable dans un `StateFlow`, changée avec `copy()` |
| **Single Activity** | Une seule `MainActivity`. **NavController** = le chauffeur (la pile d'écrans), **NavHost** = la carte des routes |
| **Lego** | Screen (branche le ViewModel) → Content (affiche) → petits composants réutilisables |

→ [[04 La couche UI (MVI et UDF)]], [[04 MainActivity]], [[00 Compose]]

---

## ✨ La qualité de code

| Notion | À retenir |
|---|---|
| **Extension** | Ajouter une **fonction** à une classe qui n'est pas à toi : `fun Context.hasPermission(...)`. `this` = l'objet sur lequel on l'appelle → [[04 Les fonctions d'extension]] |
| **KDoc** | `/** */` au-dessus d'une classe ou d'une fonction : sa **fiche d'identité**. Tags : `@param`, `@return`, `@receiver`, `@property` → [[05 KDoc et commentaires]] |
| **Commentaire `//`** | Dit **pourquoi**, pas *quoi* |
| **Lint** | **Code → Inspect Code** : relit le projet et signale les warnings. Objectif : **zéro** → [[06 Conventions de nommage et Lint]] |

---

## 🔗 Liens

- [[Fiche points faibles]] — ce que tu dois retravailler en priorité
- [[Vérification des critères]] — la checklist point par point
- [[kotlin]] — accueil du cours
