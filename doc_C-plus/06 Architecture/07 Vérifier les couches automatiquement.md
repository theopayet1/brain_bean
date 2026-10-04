---
tags:
  - projet/cpp
  - type/guide
  - techno/cmake
  - sujet/architecture
  - sujet/tests
  - statut/a-jour
aliases:
  - Test des couches
cree: 2026-10-04
maj: 2026-10-04
---

# Vérifier les couches automatiquement

> [!abstract] En une phrase
> Une règle d'architecture qui n'est pas vérifiée finit par être violée. Un petit **script CMake** lit tous les `#include "..."` de chaque couche et **échoue** si une couche inclut une couche qui n'est pas dans sa liste autorisée. Il tourne comme un test, avec `ctest`.

---

## 📄 `cmake/CheckLayers.cmake`

```cmake
# Règle d'architecture : une dépendance ne remonte jamais.
# Pour chaque couche : la liste des couches dont elle a le droit d'inclure les en-têtes.
# Usage : cmake -DSRC=<racine>/src -P CheckLayers.cmake

cmake_minimum_required(VERSION 3.28)

set(ALLOWED_domain "domain")
set(ALLOWED_application "domain;application")
set(ALLOWED_infrastructure "domain;application;infrastructure")
set(ALLOWED_bridge "domain;application;bridge")
set(ALLOWED_app "domain;application;infrastructure;bridge;app")
set(LAYERS domain application infrastructure bridge app)

set(violations "")
foreach(layer IN LISTS LAYERS)
    file(GLOB_RECURSE files "${SRC}/${layer}/*.hpp" "${SRC}/${layer}/*.cpp")
    foreach(file IN LISTS files)
        # Toutes les lignes #include "..." du fichier.
        file(STRINGS "${file}" includes REGEX "^[ \t]*#[ \t]*include[ \t]*\"")
        foreach(line IN LISTS includes)
            if(line MATCHES "#[ \t]*include[ \t]*\"([a-z_]+)/")
                set(target "${CMAKE_MATCH_1}")
                if(target IN_LIST LAYERS AND NOT target IN_LIST ALLOWED_${layer})
                    file(RELATIVE_PATH relative "${SRC}" "${file}")
                    string(APPEND violations "  ${relative} : la couche « ${layer} » inclut « ${target} »\n")
                endif()
            endif()
        endforeach()
    endforeach()
endforeach()

if(violations)
    message(FATAL_ERROR "Dépendances interdites entre couches :\n${violations}")
endif()
message(STATUS "Couches : aucune dépendance interdite.")
```

| Code | Pourquoi |
|---|---|
| `ALLOWED_<couche>` | La liste blanche de chaque couche : c'est la règle, écrite une fois |
| `file(GLOB_RECURSE ...)` | Tous les `.hpp` / `.cpp` de la couche |
| `file(STRINGS ... REGEX "#include \"")` | Seulement les lignes d'inclusion du projet (`"…"`, pas `<…>`) |
| `"([a-z_]+)/"` | Le premier dossier du chemin = la couche incluse. Ça marche parce qu'il n'y a **qu'une racine d'inclusion** (`src/`) |
| `message(FATAL_ERROR ...)` | Fait échouer le test avec la liste des fichiers fautifs |

---

## 🔌 Le brancher comme test

Dans `tests/CMakeLists.txt` :

```cmake
add_test(
    NAME layers
    COMMAND "${CMAKE_COMMAND}" "-DSRC=${CMAKE_SOURCE_DIR}/src" -P "${CMAKE_SOURCE_DIR}/cmake/CheckLayers.cmake"
)
```

`cmake -P` exécute un fichier CMake comme un script.

---

## 💥 Quand ça casse

Si quelqu'un écrit dans `src/domain/common/text.cpp` :

```cpp
#include "infrastructure/sqlite/connection.hpp"
```

```text
CMake Error at cmake/CheckLayers.cmake:33 (message):
  Dépendances interdites entre couches :

    domain/common/text.cpp : la couche « domain » inclut « infrastructure »
```

> [!tip] Deux protections qui se complètent
> - **CMake** : `bookshelf_domain` ne lie pas SQLite, donc `#include <sqlite3.h>` dans le domaine **ne compile pas** ([[02 CMake — une bibliothèque par couche]]).
> - **Ce test** : attrape les `#include` de nos propres couches, qui compileraient (tout est sous la même racine `src/`) mais casseraient l'architecture.

---

## 🔗 Liens

- [[01 Les couches et la règle des dépendances]] — la règle vérifiée ici
- [[00 Interface graphique en C++]] — la suite
