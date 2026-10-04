---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - techno/clang
  - statut/a-jour
aliases:
  - Conventions C++
  - Nommage C++
cree: 2026-10-04
maj: 2026-10-04
---

# Conventions de code C++

> [!abstract] En une phrase
> Le nommage est vérifié par **clang-tidy**, la mise en forme par **clang-format** : il reste à respecter quelques règles d'écriture et de commentaires. Code en anglais simple, commentaires en français, et chaque commentaire explique le **pourquoi**.

---

## 🏷️ Nommage

| Élément | Style | Exemple |
|---|---|---|
| Type (`class`, `struct`, `enum`, alias) | `PascalCase` | `BookService`, `NewBook`, `Result` |
| Interface | `I` + `PascalCase` | `IBooks`, `IClock` |
| Fonction, méthode | `camelCase` | `markAsRead`, `formatIso` |
| Variable, paramètre | `camelCase` | `lastRequest`, `book` |
| Membre **privé** | `camelCase_` | `books_`, `clock_` |
| Membre public d'une struct | `camelCase` | `book.title`, `book.addedOn` |
| Constante (`constexpr`, `static const`) | `PascalCase` | `MaxTitleLength`, `BookNotFound` |
| Valeur d'`enum class` | `PascalCase` | `SortOrder::Title` |
| `namespace` | `lower_case` | `bookshelf::domain` |
| Fichier | `snake_case` | `book_service.hpp`, `books_sqlite.cpp` |
| Code d'erreur (texte) | `domaine.cause` | `"book.not_found"` |
| Table, colonne SQL | `snake_case` | `book`, `added_on` |
| Implémentation d'un port | `<Chose><Techno>` | `BooksSqlite`, `SystemClock` |

---

## 📐 Mise en forme (clang-format)

- Accolades **sur leur propre ligne** (Allman), 4 espaces, 100 colonnes.
- `int* p`, `const Book& b` : le `*` et le `&` collés au **type**.
- Liste d'initialisation avec la virgule en début de ligne.
- `#include` en groupes : projet, bibliothèques, Windows, STL.
- Toujours des accolades, même pour un `if` d'une ligne.

---

## 💬 Commentaires

| Où | Quoi |
|---|---|
| Au-dessus d'une classe / fonction dans le `.hpp` | **Une phrase** : ce que c'est, à quoi ça sert, le **contrat** (« faux si… », « son `id` est ignoré ») |
| Dans le corps | Le **pourquoi** d'une ligne non évidente, avec des mots simples |
| Jamais | Paraphraser le code (`// incrémente i`) |

```cpp
// Livres dont le titre ou l'auteur contient `search` (tous si vide), triés par titre.
[[nodiscard]] virtual std::vector<domain::Book> list(std::string_view search) = 0;

// Sans ça, SQLite ignore les REFERENCES ... ON DELETE CASCADE.
execute("PRAGMA foreign_keys = ON;");
```

---

## ✅ Règles d'écriture

| Règle | Note |
|---|---|
| `const` partout où c'est possible | [[06 const, constexpr, noexcept et nodiscard]] |
| `[[nodiscard]]` sur ce qui renvoie un résultat à vérifier | idem |
| Jamais `new` / `delete`, jamais de cast C `(int)x` | [[03 Pointeurs intelligents]] |
| `enum class`, jamais `enum` nu | [[09 enum, optional et variant]] |
| Erreurs prévues : `Result<T>` ; imprévues : exceptions | [[01 Gérer les erreurs (expected et exceptions)]] |
| Pas de `using namespace` dans un `.hpp` | [[08 Fichiers hpp et cpp]] |
| Une classe, un rôle ; un fichier, une classe principale | [[01 Les couches et la règle des dépendances]] |
| Paramètres : petits par valeur, gros par `const&`, gardés par valeur + `std::move` | [[03 Fonctions]] |

---

## 🔤 TypeScript

| Élément | Style |
|---|---|
| Composant, type, interface | `PascalCase` (`Books`, `ConfirmDialogProps`) |
| Fonction, variable | `camelCase` |
| Constante globale | `UPPER_SNAKE` (`PREFIX`) |
| Fichiers de composants | `PascalCase.tsx` |
| Guillemets simples, virgule finale, 100 colonnes | Prettier |

---

## 🔗 Liens

- [[06 clang-format et clang-tidy]] — les outils qui vérifient
- [[04 Antisèche C++]] — la syntaxe
