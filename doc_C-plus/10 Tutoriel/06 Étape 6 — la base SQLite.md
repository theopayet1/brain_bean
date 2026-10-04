---
tags:
  - projet/cpp
  - type/guide
  - techno/sqlite
  - techno/doctest
  - sujet/base-de-donnees
  - statut/a-jour
aliases:
  - Tutoriel étape 6
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 6 — la base SQLite

> [!abstract] En une phrase
> On crée la couche `infrastructure` : l'enveloppe RAII de SQLite, les migrations, le dépôt `BooksSqlite` qui implémente `IBooks`, l'horloge système et le journal. Les tests tournent sur une vraie base temporaire. À lire avant : [[00 Données en C++]].

---

## 📄 1. Les fichiers

| Fichier | Note |
|---|---|
| `src/infrastructure/sqlite/connection.hpp` / `.cpp` | [[01 SQLite — enveloppe RAII]] |
| `src/infrastructure/sqlite/migrations.hpp` / `.cpp` | [[02 Migrations de schéma]] |
| `src/infrastructure/sqlite/books_sqlite.hpp` / `.cpp` | [[03 Un dépôt SQLite]] |
| `src/infrastructure/system/system_clock.hpp` / `.cpp` | [[05 L'infrastructure]] |
| `src/infrastructure/system/file_log.hpp` / `.cpp` | idem |
| `src/infrastructure/windows/data_folder.hpp` / `.cpp`, `text.hpp` / `.cpp` | idem |

`src/infrastructure/CMakeLists.txt` :

```cmake
# Infrastructure : tout ce qui touche au disque et au système.
find_package(SQLite3 REQUIRED)
# CMake 4.1 a renommé la cible SQLite::SQLite3 en SQLite3::SQLite3 : on prend celle qui existe.
if(TARGET SQLite3::SQLite3)
    set(BOOKSHELF_SQLITE_TARGET SQLite3::SQLite3)
else()
    set(BOOKSHELF_SQLITE_TARGET SQLite::SQLite3)
endif()

add_library(bookshelf_infrastructure STATIC
    sqlite/books_sqlite.cpp
    sqlite/connection.cpp
    sqlite/migrations.cpp
    system/file_log.cpp
    system/system_clock.cpp
)
add_library(bookshelf::infrastructure ALIAS bookshelf_infrastructure)

# Ce qui n'existe que sous Windows.
if(WIN32)
    target_sources(bookshelf_infrastructure PRIVATE
        windows/data_folder.cpp
        windows/text.cpp
    )
    # Shell32 : SHGetKnownFolderPath. Ole32 : CoTaskMemFree.
    target_link_libraries(bookshelf_infrastructure PRIVATE Shell32 Ole32)
endif()

target_include_directories(bookshelf_infrastructure PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_infrastructure
    PUBLIC
        bookshelf::domain
        bookshelf::application
    PRIVATE
        bookshelf::options
        ${BOOKSHELF_SQLITE_TARGET}
)
```

## 🧪 2. Les tests

- `tests/support/temp_database.hpp`
- `tests/infrastructure/books_sqlite_tests.cpp`

```cmake
bookshelf_add_tests(tests_infrastructure
    SOURCES infrastructure/books_sqlite_tests.cpp
    LIBRARIES bookshelf::infrastructure
)
```

```powershell
cmake --build --preset debug
ctest --preset debug
```

> [!tip] Regarder dans la base
> Pour voir le contenu d'une base SQLite : **DB Browser for SQLite** (gratuit), ou dans CLion la fenêtre **Database** (glisser le fichier `.db`).

---

## ✅ Point d'étape

- [ ] `tests_infrastructure` vert, sous AddressSanitizer
- [ ] Commit : « Infrastructure : SQLite »

---

## 🔗 Liens

- [[03 Tester la base de données]] — les tests en détail
- [[07 Étape 7 — le pont]] — la suite
