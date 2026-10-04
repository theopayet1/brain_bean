---
tags:
  - projet/cpp
  - type/reference
  - techno/cmake
  - techno/clion
  - sujet/build
  - statut/a-jour
aliases:
  - CMakePresets.json
cree: 2026-10-04
maj: 2026-10-04
---

# CMakePresets

> [!abstract] En une phrase
> `CMakePresets.json` range les **configurations toutes prêtes** (générateur, compilateur, toolchain vcpkg, options) sous un nom court : `cmake --preset debug` remplace une ligne de commande de 200 caractères, et **CLion / VS Code le lisent** directement.

---

## 📄 Le fichier

```json
{
  "version": 6,
  "cmakeMinimumRequired": { "major": 3, "minor": 28, "patch": 0 },
  "configurePresets": [
    {
      "name": "base",
      "hidden": true,
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "toolchainFile": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
      "architecture": { "value": "x64", "strategy": "external" },
      "environment": { "VSLANG": "1033" },
      "cacheVariables": {
        "CMAKE_CXX_COMPILER": "cl",
        "VCPKG_TARGET_TRIPLET": "x64-windows-static",
        "VCPKG_HOST_TRIPLET": "x64-windows",
        "VCPKG_INSTALLED_DIR": "${sourceDir}/vcpkg_installed"
      }
    },
    {
      "name": "debug",
      "displayName": "Debug (AddressSanitizer)",
      "inherits": "base",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "BOOKSHELF_ASAN": "ON"
      }
    },
    {
      "name": "release",
      "displayName": "Release",
      "inherits": "base",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release",
        "CMAKE_INTERPROCEDURAL_OPTIMIZATION": "ON"
      }
    },
    {
      "name": "tidy",
      "displayName": "Debug + clang-tidy",
      "inherits": "base",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug",
        "BOOKSHELF_CLANG_TIDY": "ON"
      }
    }
  ],
  "buildPresets": [
    { "name": "debug", "configurePreset": "debug" },
    { "name": "release", "configurePreset": "release" },
    { "name": "tidy", "configurePreset": "tidy" }
  ],
  "testPresets": [
    { "name": "debug", "configurePreset": "debug", "output": { "outputOnFailure": true } },
    { "name": "release", "configurePreset": "release", "output": { "outputOnFailure": true } }
  ]
}
```

---

## 🔍 Ce que fait chaque partie

| Élément | Pourquoi |
|---|---|
| `"hidden": true` sur `base` | Un preset « parent » : on ne le lance pas, les autres en **héritent** (`inherits`) |
| `"generator": "Ninja"` | Ninja compile vite et en parallèle |
| `"binaryDir": "${sourceDir}/build/${presetName}"` | Un dossier par preset : `build/debug`, `build/release` |
| `"toolchainFile": "$env{VCPKG_ROOT}/..."` | Branche vcpkg : `find_package` trouve les bibliothèques ([[04 vcpkg — les dépendances]]) |
| `"architecture": { "strategy": "external" }` | Dit à CLion / VS d'ouvrir l'environnement MSVC 64 bits |
| `"VSLANG": "1033"` | Messages du compilateur **en anglais** : plus faciles à chercher sur Internet |
| `"CMAKE_CXX_COMPILER": "cl"` | MSVC |
| `"VCPKG_TARGET_TRIPLET": "x64-windows-static"` | Bibliothèques **statiques** : un seul exe à livrer |
| `"VCPKG_INSTALLED_DIR"` | Les bibliothèques compilées sont gardées dans `vcpkg_installed/` et **partagées** entre presets |
| `debug` → `BOOKSHELF_ASAN: ON` | Le debug détecte les erreurs mémoire ([[07 Déboguer et sanitizers]]) |
| `release` → `CMAKE_INTERPROCEDURAL_OPTIMIZATION` | Optimisation à l'édition de liens : exe plus rapide et plus petit |
| `tidy` | Debug + clang-tidy sur chaque fichier ([[06 clang-format et clang-tidy]]) |
| `testPresets` → `outputOnFailure` | Un test qui échoue affiche sa sortie |

---

## ▶️ Utilisation

```powershell
cmake --list-presets             # voir les presets
cmake --preset debug             # configurer
cmake --build --preset debug     # compiler
ctest --preset debug             # tester
cmake --preset release && cmake --build --preset release   # version à livrer
```

> [!tip] `CMakeUserPresets.json`
> Pour des réglages **personnels** (un autre dossier vcpkg, un preset à toi), crée `CMakeUserPresets.json` à côté : il peut hériter des presets du projet, et il est dans le `.gitignore`.

---

## 🔗 Liens

- [[02 Installer les outils C++]] — configurer CLion pour lire les presets
- [[04 vcpkg — les dépendances]] — la suite
