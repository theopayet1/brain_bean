---
tags:
  - projet/cpp
  - type/guide
  - techno/cpp
  - techno/doctest
  - sujet/architecture
  - sujet/injection-de-dependances
  - statut/a-jour
aliases:
  - Tutoriel étape 5
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 5 — le service

> [!abstract] En une phrase
> On crée la couche `application` : les **ports** (`IBooks`, `IClock`) et le **service** `BookService` qui les utilise. On le teste avec des **faux** en mémoire : la logique « ajouter, marquer comme lu, supprimer » est prouvée avant même d'avoir une base. À lire avant : [[03 Les ports (interfaces)]] et [[04 Les services (cas d'usage)]].

---

## 📄 1. Les ports

- `src/application/ports/books.hpp` — `IBooks`
- `src/application/ports/clock.hpp` — `IClock`

## 📄 2. Le service

- `src/application/books/book_service.hpp` / `.cpp`

`src/application/CMakeLists.txt` :

```cmake
# Application : les cas d'usage et les interfaces (ports) dont ils ont besoin.
# Ne connaît ni la base, ni le système, ni l'interface : STL seule.
add_library(bookshelf_application STATIC
    books/book_service.cpp
)
add_library(bookshelf::application ALIAS bookshelf_application)

target_include_directories(bookshelf_application PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_application
    PUBLIC bookshelf::domain
    PRIVATE bookshelf::options
)
```

Et dans `src/CMakeLists.txt` : `add_subdirectory(application)` après `domain`.

## 🧪 3. Les faux et les tests

- `tests/support/fakes.hpp` — `FakeBooks`, `FixedClock`
- `tests/application/book_service_tests.cpp`

```cmake
bookshelf_add_tests(tests_application
    SOURCES application/book_service_tests.cpp
    LIBRARIES bookshelf::application
)
```

```powershell
cmake --build --preset debug
ctest --preset debug -R tests_application
```

> [!tip] Écrire le test d'abord
> Pour la prochaine fonctionnalité, essaie d'écrire le `TEST_CASE` **avant** la méthode du service : il décrit ce que tu veux, et tu sais quand tu as fini.

---

## ✅ Point d'étape

- [ ] `tests_application` vert
- [ ] `BookService` n'inclut ni SQLite, ni Windows, ni glaze
- [ ] Commit : « Application : BookService »

---

## 🔗 Liens

- [[02 Tester un service avec des faux]] — les faux en détail
- [[06 Étape 6 — la base SQLite]] — la suite
