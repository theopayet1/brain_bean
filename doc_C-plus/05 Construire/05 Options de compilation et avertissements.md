---
tags:
  - projet/cpp
  - type/reference
  - techno/cmake
  - techno/msvc
  - sujet/build
  - statut/a-jour
aliases:
  - Avertissements compilateur
  - Options MSVC
cree: 2026-10-04
maj: 2026-10-04
---

# Options de compilation et avertissements

> [!abstract] En une phrase
> Le compilateur sait repérer beaucoup de bugs (conversions qui perdent de l'info, variables non initialisées, résultats ignorés) **si on lui demande** : on active un niveau d'avertissements élevé et on transforme chaque avertissement en **erreur**. Ces options sont rangées dans **une cible CMake partagée** que toutes les couches lient.

---

## 📄 `cmake/CompilerOptions.cmake`

```cmake
# Options de compilation communes à toutes les cibles du projet.
# Chaque cible lie `bookshelf::options` en PRIVATE : rien ne fuit vers l'extérieur.

add_library(bookshelf_options INTERFACE)
add_library(bookshelf::options ALIAS bookshelf_options)

if(MSVC)
    target_compile_options(bookshelf_options INTERFACE
        /W4                 # beaucoup d'avertissements
        /permissive-        # respect strict de la norme
        /utf-8              # sources et chaînes en UTF-8 (les accents)
        /Zc:preprocessor    # préprocesseur conforme à la norme
        /EHsc               # exceptions C++
        /external:W0        # pas d'avertissement dans les en-têtes des bibliothèques
    )
    target_compile_definitions(bookshelf_options INTERFACE
        NOMINMAX            # windows.h ne définit plus les macros min et max
        WIN32_LEAN_AND_MEAN # windows.h plus léger
        UNICODE _UNICODE    # API Windows en UTF-16 (les fonctions ...W)
    )
    if(BOOKSHELF_WARNINGS_AS_ERRORS)
        target_compile_options(bookshelf_options INTERFACE /WX)
        target_link_options(bookshelf_options INTERFACE /WX)
    endif()
else()
    target_compile_options(bookshelf_options INTERFACE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        # Les initialisations désignées partielles (.title = …) sont voulues.
        -Wno-missing-field-initializers
    )
    if(BOOKSHELF_WARNINGS_AS_ERRORS)
        target_compile_options(bookshelf_options INTERFACE -Werror)
    endif()
endif()

if(BOOKSHELF_ASAN)
    if(MSVC)
        target_compile_options(bookshelf_options INTERFACE /fsanitize=address)
        # ASan n'accepte pas l'édition de liens incrémentale.
        target_link_options(bookshelf_options INTERFACE /INCREMENTAL:NO)
        # Les bibliothèques vcpkg ne sont pas compilées avec ASan : sans ces deux
        # définitions, l'éditeur de liens refuse le mélange.
        target_compile_definitions(bookshelf_options INTERFACE
            _DISABLE_VECTOR_ANNOTATION
            _DISABLE_STRING_ANNOTATION
        )
    else()
        target_compile_options(bookshelf_options INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(bookshelf_options INTERFACE -fsanitize=address,undefined)
    endif()
endif()

if(BOOKSHELF_CLANG_TIDY)
    find_program(BOOKSHELF_CLANG_TIDY_EXE clang-tidy REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY "${BOOKSHELF_CLANG_TIDY_EXE}")
    if(MSVC)
        # Sans /EHsc, clang-tidy croit les exceptions désactivées et refuse chaque try.
        list(APPEND CMAKE_CXX_CLANG_TIDY "--extra-arg=/EHsc")
    endif()
endif()
```

---

## 🔍 Les options MSVC

| Option | Effet |
|---|---|
| `/W4` | Niveau d'avertissements élevé (le maximum utile ; `/Wall` est trop bavard) |
| `/WX` | Un avertissement **arrête** le build |
| `/permissive-` | Refuse les extensions non standard de MSVC |
| `/utf-8` | Sources et chaînes en UTF-8 : les accents restent justes |
| `/Zc:preprocessor` | Préprocesseur conforme (nécessaire à certaines bibliothèques) |
| `/EHsc` | Exceptions C++ standard |
| `/external:W0` | Silence dans les en-têtes **des bibliothèques** (on ne peut pas les corriger) |
| `NOMINMAX` | Sinon `windows.h` définit des macros `min` / `max` qui cassent `std::min` |
| `WIN32_LEAN_AND_MEAN` | `windows.h` plus léger |
| `UNICODE` / `_UNICODE` | Les fonctions Windows sans suffixe pointent vers les versions UTF-16 (`...W`) |

## 🔍 Les options GCC / Clang

| Option | Effet |
|---|---|
| `-Wall -Wextra -Wpedantic` | Le jeu d'avertissements standard |
| `-Wconversion` | Conversions qui peuvent perdre des données (`int64_t` → `int`) |
| `-Wshadow` | Une variable en cache une autre du même nom |
| `-Wno-missing-field-initializers` | Les initialisations désignées partielles (`{.title = "x"}`) sont **voulues** |
| `-Werror` | Avertissement = erreur |

---

## 🎯 Pourquoi une cible `INTERFACE` liée en `PRIVATE`

```cmake
add_library(bookshelf_options INTERFACE)       # pas de source : juste des réglages
target_link_libraries(bookshelf_domain PRIVATE bookshelf::options)
```

- Chaque cible du projet reçoit les mêmes options.
- `PRIVATE` : les options ne « fuient » pas vers les cibles externes (CMakeRC, les bibliothèques vcpkg) qu'on ne contrôle pas.

---

## 🧯 Faire taire un avertissement, proprement

1. **Corriger** le code. C'est presque toujours la bonne réponse.
2. Si l'avertissement est faux pour **une ligne** : un commentaire qui explique pourquoi + la suppression la plus étroite possible.
   ```cpp
   const auto length = static_cast<int>(text.size());   // conversion VOULUE et visible
   ```
3. Jamais `/W0`, jamais `/WX-` « pour que ça compile ».

---

## 🔗 Liens

- [[06 clang-format et clang-tidy]] — l'analyse va plus loin que le compilateur
- [[07 Déboguer et sanitizers]] — l'option `BOOKSHELF_ASAN`
