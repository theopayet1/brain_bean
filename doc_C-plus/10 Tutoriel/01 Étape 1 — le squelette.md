---
tags:
  - projet/cpp
  - type/guide
  - techno/cmake
  - techno/vcpkg
  - sujet/build
  - statut/a-jour
aliases:
  - Tutoriel étape 1
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 1 — le squelette

> [!abstract] En une phrase
> On pose la structure du projet — CMake, presets, vcpkg, options de compilation, formatage — et on fait passer **un premier test**. Après cette étape, tout le reste n'est qu'ajout de fichiers. À lire avant : [[02 Installer les outils C++]].

---

## 📂 1. Créer les dossiers

```powershell
mkdir bookshelf; cd bookshelf
git init
mkdir cmake, scripts, src\domain\common, tests\domain, frontend
```

## 📄 2. Les fichiers de la racine

Copier depuis `_projet-exemple/bookshelf/` :

| Fichier | Rôle | Note |
|---|---|---|
| `CMakeLists.txt` | Le projet | [[02 CMake — une bibliothèque par couche]] |
| `CMakePresets.json` | debug / release / tidy | [[03 CMakePresets]] |
| `vcpkg.json` | Les bibliothèques | [[04 vcpkg — les dépendances]] |
| `cmake/CompilerOptions.cmake` | `/W4 /WX`, ASan | [[05 Options de compilation et avertissements]] |
| `cmake/CheckLayers.cmake` | Le test des couches | [[07 Vérifier les couches automatiquement]] |
| `.clang-format`, `.clang-tidy`, `.editorconfig`, `.gitignore` | Style, analyse, éditeur, git | [[06 clang-format et clang-tidy]] |

> [!warning] Pour cette étape seulement
> L'application graphique n'existe pas encore : configure avec `-DBOOKSHELF_APP=OFF`, ou retire temporairement `include(Frontend)` et `add_subdirectory(app)`. On les rebranche aux étapes 2 et 3.

## 📄 3. Une première couche, un premier test

`src/CMakeLists.txt` (pour l'instant) :

```cmake
set(BOOKSHELF_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
add_subdirectory(domain)
```

`src/domain/CMakeLists.txt` avec un seul fichier, `common/text.cpp` ([[02 Le domaine]]) :

```cmake
add_library(bookshelf_domain STATIC
    common/text.cpp
)
add_library(bookshelf::domain ALIAS bookshelf_domain)
target_include_directories(bookshelf_domain PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_domain PRIVATE bookshelf::options)
```

`src/domain/common/text.hpp` et `text.cpp` : la fonction `trim` du projet modèle.

`tests/main.cpp` :

```cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
```

`tests/domain/text_tests.cpp` :

```cpp
#include "domain/common/text.hpp"

#include <doctest/doctest.h>

TEST_CASE("trim retire les espaces autour")
{
    CHECK(bookshelf::domain::trim("  Dune  ") == "Dune");
    CHECK(bookshelf::domain::trim("   ").empty());
}
```

`tests/CMakeLists.txt` : la fonction `bookshelf_add_tests` du modèle ([[00 Tests C++]]) avec un seul exe :

```cmake
bookshelf_add_tests(tests_domain
    SOURCES domain/text_tests.cpp
    LIBRARIES bookshelf::domain
)
```

## ▶️ 4. Construire et tester

Dans le Developer PowerShell :

```powershell
$env:VCPKG_ROOT            # doit afficher le chemin de vcpkg
cmake --preset debug       # la 1re fois : vcpkg compile les bibliothèques (long)
cmake --build --preset debug
ctest --preset debug
```

```text
100% tests passed, 0 tests failed out of 2
```

(2 tests : `tests_domain` et `layers`, si tu as aussi copié le `add_test(NAME layers ...)` du modèle.)

---

## ✅ Point d'étape

- [ ] `cmake --preset debug` sans erreur
- [ ] `ctest --preset debug` vert
- [ ] Premier commit : `git add -A && git commit -m "Squelette du projet"`

---

## 🔗 Liens

- [[00 Tutoriel — une app de bureau complète]] — le plan
- [[02 Étape 2 — la fenêtre]] — la suite
