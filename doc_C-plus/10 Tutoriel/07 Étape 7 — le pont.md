---
tags:
  - projet/cpp
  - type/guide
  - techno/cpp
  - techno/glaze
  - techno/saucer
  - sujet/ui
  - statut/a-jour
aliases:
  - Tutoriel étape 7
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 7 — le pont

> [!abstract] En une phrase
> On crée la couche `bridge` (DTO, messages, `Bridge`, `Worker`), on la teste **sans fenêtre**, puis on **assemble tout** dans le `main` : base, service, pont, fenêtre, fil de travail. À la fin, le JavaScript peut appeler le C++. À lire avant : [[05 Le pont C++ JavaScript — le protocole]].

---

## 📄 1. La couche `bridge`

| Fichier | Note |
|---|---|
| `src/bridge/dto.hpp` | [[06 Le pont côté C++]] |
| `src/bridge/messages.hpp` / `.cpp` | idem |
| `src/bridge/bridge.hpp` / `.cpp` | idem |
| `src/bridge/worker.hpp` / `.cpp` | [[05 Threads et file de tâches]] |
| `src/bridge/CMakeLists.txt` | [[02 CMake — une bibliothèque par couche]] |

## 🧪 2. Les tests

- `tests/bridge/bridge_tests.cpp` — [[04 Tester le pont]]
- `tests/bridge/worker_tests.cpp` — [[08 Le fil de travail]]

```cmake
bookshelf_add_tests(tests_bridge
    SOURCES bridge/bridge_tests.cpp bridge/worker_tests.cpp
    LIBRARIES bookshelf::bridge
)
```

## 🔌 3. Tout assembler

- `src/app/main.cpp` : la version complète du modèle ([[06 La racine de composition]]).
- `src/app/CMakeLists.txt` : ajouter `bookshelf::infrastructure` et `bookshelf::bridge`.

## 🧪 4. Essayer depuis la console de la page

Lancer l'appli, `F12`, onglet **Console** :

```js
window.__bookshelf_response = (r) => console.log(r);
window.chrome.webview.postMessage('bookshelf:' + JSON.stringify({
  id: 1, function: 'addBook', request: { title: 'Dune', author: 'Frank Herbert', year: 1965 },
}));
```

```text
{id: 1, ok: true, data: {id: 1}}
```

Le C++ a répondu, et `bookshelf.db` est apparu dans `%LOCALAPPDATA%\Bookshelf\`.

---

## ✅ Point d'étape

- [ ] `tests_bridge` vert
- [ ] L'appel depuis la console fonctionne
- [ ] Commit : « Pont et assemblage »

---

## 🔗 Liens

- [[08 Le fil de travail]] — le branchement en détail
- [[08 Étape 8 — les écrans]] — la suite
