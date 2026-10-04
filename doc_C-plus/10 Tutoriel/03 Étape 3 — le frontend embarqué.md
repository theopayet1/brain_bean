---
tags:
  - projet/cpp
  - type/guide
  - techno/vite
  - techno/preact
  - techno/cmake
  - sujet/ui
  - statut/a-jour
aliases:
  - Tutoriel étape 3
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 3 — le frontend embarqué

> [!abstract] En une phrase
> On crée le projet `frontend/` (Vite + TypeScript + Preact), on le fait construire par CMake, on l'embarque avec CMakeRC, et la fenêtre l'affiche via `app://bookshelf/index.html`. L'appli fonctionne **sans réseau**. À lire avant : [[03 Le frontend (Vite, TypeScript, Preact)]] et [[04 Embarquer le frontend dans l'exe]].

---

## 📄 1. Le projet web

Dans `frontend/`, copier depuis le modèle : `package.json`, `vite.config.ts`, `tsconfig.json`, `eslint.config.js`, `.prettierrc.json`, `.prettierignore`, `index.html`.

Puis installer les dépendances (crée `package-lock.json`, **à commiter**) :

```powershell
cd frontend
npm install preact
npm install -D vite typescript eslint @eslint/js typescript-eslint prettier
```

Un premier `src/main.tsx` :

```tsx
import { render } from 'preact';

function App() {
  return <h1>Bonjour depuis Preact</h1>;
}

const root = document.querySelector<HTMLDivElement>('#app');
if (!root) {
  throw new Error('Élément #app introuvable dans index.html');
}
render(<App />, root);
```

Essayer dans le navigateur : `npm run dev`. Puis `npm run build` doit créer `dist/`.

## 📄 2. L'embarquer

- Copier `cmake/Frontend.cmake` et `cmake/CheckDist.cmake`.
- Copier `src/app/embedded_frontend.hpp` / `.cpp` et `src/app/window.hpp` / `.cpp`.
- `src/app/CMakeLists.txt` : la version complète du modèle **sans** `bookshelf::infrastructure` ni `bookshelf::bridge` pour l'instant :

```cmake
include(Saucer)
configure_file(identity.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/generated/app/identity.hpp" @ONLY)
add_executable(bookshelf WIN32 embedded_frontend.cpp main.cpp window.cpp)
target_include_directories(bookshelf PRIVATE "${BOOKSHELF_SRC_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/generated")
target_link_libraries(bookshelf PRIVATE bookshelf::options bookshelf::frontend saucer::saucer)
set_target_properties(bookshelf PROPERTIES OUTPUT_NAME "${BOOKSHELF_APP_NAME}")
```

## 📄 3. Le `main` sert `app://`

```cpp
saucer::webview::register_scheme(std::string{app::FrontendScheme});    // 👈 avant init
auto application = saucer::application::init({.id = "bookshelf"});

saucer::webview window{{
    .application = application,
    .persistent_cookies = false,
    .browser_flags = app::browserFlags(),                               // 👈
}};
// ... titre, taille ...
app::hardenWebView(window);                                             // 👈
window.handle_scheme(std::string{app::FrontendScheme}, app::serveFrontend);   // 👈
window.on<saucer::web_event::navigate>(                                 // 👈
    [](const saucer::navigation& navigation)
    {
        return app::isFrontendUrl(navigation.url()) ? saucer::policy::allow
                                                    : saucer::policy::block;
    });
window.set_url(std::string{app::HomePage});                             // 👈 app://bookshelf/index.html
```

## ▶️ 4. Lancer

```powershell
cmake --preset debug
cmake --build --preset debug
.\build\debug\src\app\Bookshelf.exe
```

La fenêtre affiche « Bonjour depuis Preact ». Couper le Wi-Fi : ça marche toujours.

Modifier `main.tsx`, relancer `cmake --build --preset debug` : Vite reconstruit, l'exe est relié avec la nouvelle version.

---

## ⚠️ Si ça ne marche pas

| Symptôme | Cause | Solution |
|---|---|---|
| Page blanche, console : `Failed to load ... /assets/index.js` | `base: './'` manquant | Voir `vite.config.ts` |
| `npm` introuvable | Node.js pas dans le `PATH` du terminal de CLion | Redémarrer CLion après l'installation de Node |
| Le frontend ne se met pas à jour | `dist/` contient d'anciens fichiers | Supprimer `frontend/dist/`, reconfigurer |

---

## ✅ Point d'étape

- [ ] La fenêtre affiche la page Preact, hors ligne
- [ ] `ctest` : le test `frontend_offline` est vert
- [ ] Commit : « Frontend embarqué »

---

## 🔗 Liens

- [[09 Sécuriser la webview]] — ce que font `browserFlags` et `hardenWebView`
- [[04 Étape 4 — le domaine]] — la suite
