---
tags:
  - projet/cpp
  - type/guide
  - techno/saucer
  - techno/webview2
  - sujet/ui
  - statut/a-jour
aliases:
  - Tutoriel étape 2
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 2 — la fenêtre

> [!abstract] En une phrase
> On ajoute la couche `app` avec un `main` minimal : une fenêtre saucer qui affiche une page web. C'est la preuve que saucer, WebView2 et l'édition de liens fonctionnent. À lire avant : [[02 saucer — ouvrir une fenêtre]].

---

## 📄 1. Rebrancher saucer

- Copier `cmake/Saucer.cmake` ([[04 vcpkg — les dépendances]]).
- `src/CMakeLists.txt` : ajouter `if(BOOKSHELF_APP) add_subdirectory(app) endif()`.
- Reconfigurer **sans** `-DBOOKSHELF_APP=OFF`.

## 📄 2. Un `main` minimal

`src/app/main.cpp` :

```cpp
#include <saucer/webview.hpp>

#include <windows.h>

int WINAPI wWinMain(HINSTANCE /*instance*/, HINSTANCE /*previous*/, PWSTR /*commandLine*/, int /*show*/)
{
    auto application = saucer::application::init({.id = "bookshelf"});

    saucer::webview window{{.application = application}};
    window.set_title("Bookshelf");
    window.set_size(1100, 720);
    window.set_dev_tools(true);
    window.set_url("https://example.com");    // provisoire : à l'étape 3, ce sera app://

    window.show();
    application->run();
    return 0;
}
```

`src/app/CMakeLists.txt` (provisoire) :

```cmake
include(Saucer)

add_executable(bookshelf WIN32 main.cpp)
target_link_libraries(bookshelf PRIVATE bookshelf::options saucer::saucer)
set_target_properties(bookshelf PROPERTIES OUTPUT_NAME "${BOOKSHELF_APP_NAME}")
```

## ▶️ 3. Lancer

```powershell
cmake --preset debug
cmake --build --preset debug
.\build\debug\src\app\Bookshelf.exe
```

Une fenêtre s'ouvre avec la page `example.com`. `F12` ouvre les outils de développement.

---

## ⚠️ Si ça ne marche pas

| Symptôme | Cause | Solution |
|---|---|---|
| `Could NOT find saucer` / `find_library ... saucer` | `saucer` absent de `vcpkg.json` | L'ajouter, reconfigurer |
| `LNK2019` sur des symboles `Dwm...`, `Gdiplus...` | Bibliothèques Windows non liées | Utiliser le `Saucer.cmake` du modèle, complet |
| La fenêtre s'ouvre et se ferme | Exception au démarrage | Lancer depuis CLion en debug pour voir où |
| Page blanche | Pas d'Internet (normal pour example.com) | Rien de grave : l'étape 3 rend l'appli hors ligne |

---

## ✅ Point d'étape

- [ ] La fenêtre s'ouvre
- [ ] Commit : « Fenêtre saucer »

---

## 🔗 Liens

- [[01 Étape 1 — le squelette]] — l'étape d'avant
- [[03 Étape 3 — le frontend embarqué]] — la suite
