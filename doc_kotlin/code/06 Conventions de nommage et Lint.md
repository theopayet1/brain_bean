---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - techno/gradle
  - sujet/conventions
  - sujet/qualite
  - sujet/debug
  - statut/a-jour
cours: P1
cree: 2026-10-26
maj: 2026-10-28
---

# Conventions de nommage et Lint

> [!abstract] En une phrase
> Les **conventions de nommage** disent **comment écrire** chaque nom (`PascalCase`, `camelCase`…) ; **Lint** est l'outil d'Android Studio qui **relit ton projet** et signale les **warnings**. La grille veut des noms qui suivent les conventions Kotlin, et un manifest et un Gradle **sans warning**.

> [!note] Complété le 26 octobre 2026
> Note ajoutée à partir de la grille d'évaluation (SE-5 « conventions de nommage de Kotlin respectées (Lint) », SF-5 « Manifest / Gradle : warning interdit »).

---

## 🏷️ Les conventions Kotlin

| Quoi | Convention | Exemple |
|---|---|---|
| **Package** | minuscules, sans `_` | `com.diiage.metrix.ui.screens` |
| **Classe**, interface, `object` | `PascalCase` (un nom) | `HostRepository`, `VibrationManager` |
| **Fonction** | `camelCase` (un **verbe**) | `loadHosts()`, `resetFilters()` |
| **Composable** | `PascalCase` (un **nom**, comme une classe) | `HostListScreen()`, `HostCard()` |
| **Variable**, propriété | `camelCase` | `isLoading`, `selectedStatuses` |
| **Booléen** | commence par `is`, `has`, `can` | `isDisabled`, `hasCustomFilters`, `canManage` |
| **Constante** (`const val`) | `SCREAMING_SNAKE_CASE` | `DEFAULT_HOST_PORT` |
| **Propriété « cachée »** (*backing property*) | `_` devant | `_uiState` (privé) / `uiState` (public) |
| **Fichier** | le nom de **sa classe principale** | `HostListViewModel.kt` |

> [!info] Tous ces exemples viennent de Metrix
> Metrix suit déjà la plupart de ces règles. Exception : la classe `exemple` dans `domain/usecase/exemple.kt` commence par une **minuscule** → Android Studio la souligne.

### Les suffixes d'une app Android

Le **suffixe** dit **le rôle** de la classe : on sait ce qu'elle fait sans l'ouvrir.

| Suffixe | Rôle | Note |
|---|---|---|
| `…Screen` / `…Content` | Un écran (avec / sans ViewModel) | [[04 La couche UI (MVI et UDF)]] |
| `…ViewModel` / `…UiState` / `…Action` | La logique, l'état, les actions d'un écran | [[04 La couche UI (MVI et UDF)]] |
| `…Repository` | Le contrat ou l'implémentation d'accès aux données | [[02 La couche Domain]] |
| `…Dto` / `…Entity` / `…Dao` | La couche Data | [[03 La couche Data]] |
| `…Manager` | L'accès à une capacité native | [[04 Les capacités natives (Managers)]] |
| `…Extensions.kt` | Un fichier de fonctions d'extension | [[04 Les fonctions d'extension]] |

> [!success] Le préfixe `I` et les abréviations (tranché le 28 octobre 2026)
> On voit souvent `IHostRep` (interface) et `HostRepo` (implémentation). Le `I` devant une interface est une habitude **C#**, et les conventions Kotlin déconseillent les **abréviations** (`Rep`, `Repo`). Lint ne le signale pas, mais le prof y fait attention. La doc utilise donc :
> - **`HostRepository`** pour l'**interface** (dans `domain/repository/`) ;
> - **`HostRepositoryImpl`** pour l'**implémentation** (dans `data/repository/`).
>
> Pour renommer dans un projet : clic droit sur la classe → **Refactor → Rename**, Android Studio met à jour tous les usages.

---

## 🔍 Lint : trouver les warnings

**Lint** relit **tout le projet** (code, manifest, Gradle, ressources) et classe ce qu'il trouve en **erreurs** et **warnings**.

Trois façons de le lancer :

1. **Dans l'éditeur** : les warnings sont **surlignés en jaune**, les erreurs en rouge. Survole pour lire le message, **Alt + Entrée** pour la correction proposée.
2. **Code → Inspect Code…** → **Whole project** : la liste complète, rangée par catégorie, dans l'onglet **Problems**.
3. **Terminal** :

```bash
.\gradlew lint
```

Le rapport s'ouvre dans un navigateur : `app/build/reports/lint-results-debug.html`.

> [!tip] Avant le rendu
> Lance **Inspect Code** sur tout le projet, et vise **zéro warning** dans `AndroidManifest.xml`, `build.gradle.kts` et `libs.versions.toml`. La grille passe de « En cours d'acquisition » (warning toléré) à « Maîtrisé » (warning interdit) **là-dessus**.

---

## 🧯 Les warnings fréquents

| Symptôme | Cause | Solution |
|---|---|---|
| `A newer version of … is available` (dans le catalogue) | La lib a une version plus récente | Mettre à jour la version dans `libs.versions.toml`, puis **Sync** |
| `Use version catalog instead` | Une dépendance **écrite en dur** dans `build.gradle.kts` (`"group:name:1.2.3"`) | La déplacer dans le catalogue → [[02 Ajouter une dépendance]] |
| `kotlinOptions` barré ou souligné | Ancienne écriture ([[03 Le bloc android]]) | La remplacer par le bloc `kotlin { compilerOptions { … } }` (ci-dessous) |
| `android:exported needs to be explicitly specified` | Une activity avec `intent-filter` sans `exported` | Ajouter `android:exported="true"` → [[02 Les Intents]] |
| `Missing permissions required by …` | On appelle un service protégé (vibreur, GPS) sans la permission | La déclarer dans le manifest → [[03 Les Permissions]] |
| `Hardcoded string "…", should use @string resource` | Un texte écrit en dur dans un fichier XML | Le mettre dans `strings.xml` → [[4.1 Les ressources en pratique]] |
| `The resource R.… appears to be unused` | Une ressource que plus personne n'utilise | La supprimer |
| `Class name 'exemple' should start with an uppercase letter` | Nom de classe hors convention | **Refactor → Rename** (**Maj + F6**) |
| `Unnecessary; SDK_INT is always >= 29` | Un test de version inutile vu le `minSdk` | Supprimer le `if` |

### Remplacer `kotlinOptions`

```kotlin
// app/build.gradle.kts — en haut du fichier
import org.jetbrains.kotlin.gradle.dsl.JvmTarget

// …en dehors du bloc android { }
kotlin {
    compilerOptions {
        jvmTarget.set(JvmTarget.JVM_11)
    }
}
```

| Code | Pourquoi |
|---|---|
| `kotlin { compilerOptions { … } }` | La nouvelle écriture, **à côté** du bloc `android { }`, plus dedans |
| `JvmTarget.JVM_11` | Même valeur qu'avant (`"11"`), mais **typée** : une faute de frappe ne compile plus |

---

## 🔗 Liens

- [[00 Code]]
- [[05 KDoc et commentaires]] — l'autre moitié de la qualité
- [[04 Commandes et erreurs Gradle]] — les erreurs de **build** (celles qui bloquent)
- [[4.1 Les ressources en pratique]] — nommer les ressources
