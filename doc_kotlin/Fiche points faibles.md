---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - sujet/evaluation
  - statut/a-jour
aliases:
  - Points faibles
  - Fiche de révision ciblée
cours: P1
cree: 2026-10-28
maj: 2026-10-28
---

# Fiche points faibles

> [!abstract] En une phrase
> Les points où tu as **coincé au diagnostic du 27 octobre 2026**. Pour chacun : **ce que tu as dit**, **ce qui manquait**, **la réponse à réciter** et **une question pour te tester**. La fiche générale est dans [[Fiche de révision]].

> [!tip] Comment la travailler
> Lis la **réponse à réciter**, cache-la, puis réponds à voix haute à la **question test**. Quand tu y arrives sans regarder, coche la case.

---

## 🟠 Les 5 points à passer au vert

### 1. Le mécanisme de fetch (S-2)

- [ ] Je sais l'expliquer sans regarder

> [!failure] Ce que tu as dit
> « Le Repository tape l'API, et si erreur, il va chercher dans Room. »
> **Problème** : personne ne **remplit** Room. Hors ligne, la base est vide.

> [!success] La réponse à réciter
> « En **offline-first**, Room est la **seule source de vérité**. L'écran lit **toujours** Room via un `Flow`, donc il affiche tout de suite les dernières données, même hors ligne. En parallèle, le Repository appelle l'API et **range** le résultat dans Room (`toEntity()` puis `upsertAll`). Le `Flow` prévient l'écran tout seul. »

```
écran ◄── Flow ◄── Room ◄── refresh ◄── API
```

**Question test** : « L'utilisateur est dans le métro, sans réseau. Qu'est-ce qu'il voit, et pourquoi ? »

→ [[03 La couche Data]] · *Le mécanisme de fetch*

---

### 2. Qui fait quoi pour le matériel (S-6)

- [ ] Je sais le dessiner sans regarder

> [!failure] Ce que tu as dit
> Tu as bien expliqué les permissions, mais pas **le chemin dans le code**.

> [!success] La réponse à réciter
> « L'**écran** envoie une action au **ViewModel**. Le ViewModel décide **quand** et appelle le **Manager**. Le Manager sait **comment** : il demande le service au **Context** avec `getSystemService`. Koin donne l'Application Context au Manager, et le Manager au ViewModel. L'écran n'importe **jamais** rien de matériel. »

```
UI       Screen ──action──► ViewModel (QUAND)
                                │
Data                        Manager (COMMENT)
                                │ getSystemService
Android                     Context ──► vibreur, GPS…
```

**Question test** : « Pourquoi on ne fait pas `getSystemService` directement dans le composable ? » (3 raisons : lisibilité, réutilisation, preview / test)

→ [[4.1 Le vibreur pas à pas]], [[04 Les capacités natives (Managers)]] · *La solution*

---

### 3. La rotation (S-5)

- [ ] Je sais l'expliquer sans regarder

> [!failure] Ce que tu as dit
> « On fait un `onPause`, on reconstruit l'écran, puis `onResume`. »
> **Problème** : c'est beaucoup plus violent que ça.

> [!success] La réponse à réciter
> « Par défaut, une rotation (ou le mode nuit, ou un changement de langue) **détruit l'activity puis la recrée** : `onPause → onStop → onDestroy`, puis `onCreate → onStart → onResume`. Les variables et `remember` sont **perdus**. Ce qui survit : le **ViewModel** et `rememberSaveable`. »

**Question test** : « Ma liste d'hôtes se recharge à chaque fois que je tourne le téléphone. Pourquoi, et comment je corrige ? » (elle est chargée dans l'activity → la mettre dans le ViewModel)

→ [[03 Le cycle de vie]] · *La rotation*

---

### 4. Le contenu du manifest (S-5)

- [ ] Je sais le réciter sans regarder

> [!failure] Ce que tu as dit
> « La carte d'identité de l'application. » C'est le bon mot, mais **sans le contenu**.

> [!success] La réponse à réciter
> « C'est la **carte d'identité** de l'app. Il déclare le **nom** et l'**icône**, les **permissions** (`VIBRATE`, `INTERNET`, `ACCESS_FINE_LOCATION`), les **activities**, dont la principale avec l'`intent-filter` `MAIN` / `LAUNCHER`, et la classe `Application`. Ce n'est pas lui le gardien : c'est **Android** qui le lit et bloque ce qui n'est pas déclaré. »

**Question test** : « J'ai oublié `<uses-permission android:name="android.permission.INTERNET" />`. Que se passe-t-il quand l'app appelle l'API ? »

→ [[01 Le manifest]]

---

### 5. Écrire une fonction d'extension (SE-5)

- [ ] J'arrive à l'écrire de tête, KDoc compris

> [!failure] Ce que tu as dit
> La définition de la note, mais « j'ai pas trop compris ». Et une erreur : on ajoute une **fonction**, pas une **classe**.

> [!success] La réponse à réciter
> « Une extension permet d'**ajouter une fonction à une classe qui n'est pas à moi**, sans la modifier. J'écris `fun Context.hasPermission(p: String)`, et dedans, `this` est l'objet sur lequel on l'appelle. Ensuite j'écris `context.hasPermission(...)` comme si la méthode avait toujours existé. Dans le KDoc, je documente le `this` avec `@receiver`. »

```kotlin
/**
 * Vérifie si l'app a déjà reçu une permission.
 *
 * @receiver le Context (Activity ou Application) sur lequel on vérifie.
 * @param permission la permission à tester, ex. `Manifest.permission.CAMERA`.
 * @return `true` si la permission est accordée.
 */
fun Context.hasPermission(permission: String): Boolean =
    ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED
```

> [!example] L'image
> Un **autocollant** sur un objet qui n'est pas à toi : l'objet n'est pas modifié, mais il a un bouton en plus.

**Question test** : écris de tête une extension `Context.toast(message: String)` qui affiche un `Toast`, avec son KDoc.

→ [[04 Les fonctions d'extension]]

---

## 🟡 Les petits pièges à ne pas refaire

Tu sais déjà ces sujets, mais tu as glissé sur un détail. À l'oral, c'est ce genre de détail qui fait la différence entre Acquis et Maîtrisé.

| Piège | Ce que tu as dit | Ce qu'il faut dire |
|---|---|---|
| **La flèche Data → Domain** | `data <- domaine` | **`UI → Domain ← Data`** : Data pointe **vers** le Domain → [[00 Architecture]] |
| **Visible ≠ arrière-plan** | « visible = l'app est en arrière-plan » | Visible = **affichée mais pas au premier plan** (dialogue par-dessus). Arrière-plan = **purgatoire** → [[03 Le cycle de vie]] |
| **Le Context dans Koin** | « jamais dans Koin » puis « dans un module Koin » | L'**Application Context** dans Koin, c'est OK (`androidContext()`). C'est l'**Activity Context** qu'on ne garde jamais → [[01 Le Context]] |
| **NavController / NavHost** | « le NavHost dit à la barre où on est » | **NavController** = le chauffeur (`navigate`, la pile). **NavHost** = la carte (route → composable) → [[04 MainActivity]] |
| **Pourquoi Kotlin pur** | « Android n'est pas de la logique métier » | Ajoute **testable sans émulateur**, **indépendant des frameworks**, **réutilisable** → [[02 La couche Domain]] |
| **Mapping** | « vers le modèle data » | Vers le **modèle du Domain** : `HostDto → Host` → [[03 La couche Data]] |
| **Le son et les permissions** | « jouer un son = permission simple » | Jouer un son ne demande **aucune** permission. Exemple de permission normale : **`VIBRATE`** → [[03 Les Permissions]] |
| **Intent de MVI** | — | L'Intent de MVI (intention de l'utilisateur) **≠** l'Intent Android (message au système) |

---

## 🔗 Liens

- [[Fiche de révision]] — tout le cours en une page
- [[Vérification des critères]] — la checklist complète de la grille
