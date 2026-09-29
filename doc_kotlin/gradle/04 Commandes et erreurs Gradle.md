---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - techno/gradle
  - sujet/build
  - sujet/debug
  - statut/a-jour
cours: P1
cree: 2026-09-29
maj: 2026-09-29
---

# Commandes et erreurs Gradle

> [!abstract] En une phrase
> Les **commandes** pour lancer Gradle à la main, et un **tableau des erreurs** les plus fréquentes avec leur solution.

---

## ⌨️ Les commandes `gradlew`

`gradlew` (le **wrapper**) est à la racine du projet. On le lance dans le **terminal** d'Android Studio (onglet *Terminal*, en bas).

| Commande (Windows) | Ce qu'elle fait |
|---|---|
| `.\gradlew assembleDebug` | Fabrique l'APK de debug (`app/build/outputs/apk/debug/`) |
| `.\gradlew installDebug` | Fabrique **et installe** l'app sur le téléphone ou l'émulateur branché |
| `.\gradlew clean` | Supprime le dossier `build/` (repartir de zéro) |
| `.\gradlew :app:dependencies` | Affiche **l'arbre des dépendances** (qui tire quelle version) |
| `.\gradlew assembleDebug --stacktrace` | Même chose, avec le **détail** de l'erreur si ça plante |

> [!tip] Mac / Linux
> Même chose avec `./gradlew` au lieu de `.\gradlew`.

> [!note] Pas besoin d'installer Gradle
> Le wrapper télécharge **tout seul** la version écrite dans `gradle/wrapper/gradle-wrapper.properties` (Gradle 8.13 pour Metrix). Tout le monde utilise donc la même version.

---

## 🧯 Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| Nouvel `import` en rouge après avoir ajouté une lib | Pas de Sync | **Sync Now** |
| `Unresolved reference: libs` ou `libs.xxx` en rouge | Faute dans le nom, ou lib absente du catalogue | Vérifier `libs.versions.toml` : **tirets → points** |
| `Could not resolve …` / `Could not find …` | Pas d'internet, version qui n'existe pas, ou faute dans `group` / `name` | Vérifier la connexion et la version dans la doc officielle de la lib |
| `Plugin [id: '…'] was not found` | Plugin absent de `[plugins]`, ou faute dans l'`id` | Le déclarer dans le catalogue, puis `alias(libs.plugins.…)` |
| `Android Gradle plugin requires Java 17 to run` | Gradle tourne avec un JDK trop ancien | **Settings → Build, Execution, Deployment → Build Tools → Gradle → Gradle JDK** : choisir un JDK 17 ou plus récent (celui d'Android Studio, `jbr`, convient) |
| `Dependency '…' requires compileSdk 36 or later` | Une lib est plus récente que le `compileSdk` | Augmenter `compileSdk` → [[03 Le bloc android]] |
| `SDK location not found` | `local.properties` absent (projet fraîchement cloné) | Ouvrir le projet dans Android Studio : il recrée le fichier |
| `Duplicate class …` | La même lib arrive deux fois avec deux versions | `.\gradlew :app:dependencies` pour trouver le doublon, puis aligner les versions (un BOM aide) |
| Le build marchait, et plus rien ne marche sans raison | Cache corrompu | `.\gradlew clean`, puis **File → Invalidate Caches → Invalidate and Restart** |

---

## 🔗 Liens

- [[00 Gradle]]
- [[02 Ajouter une dépendance]]
- [[03 Les logs]] — pour les erreurs **à l'exécution** (et non à la compilation)
