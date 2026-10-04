---
tags:
  - projet/cpp
  - type/reference
  - techno/clang
  - techno/clion
  - sujet/build
  - statut/a-jour
aliases:
  - clang-format
  - clang-tidy
cree: 2026-10-04
maj: 2026-10-04
---

# clang-format et clang-tidy

> [!abstract] En une phrase
> **clang-format** remet le code en forme tout seul (indentation, accolades, ordre des `#include`) : plus jamais de débat de style. **clang-tidy** lit le code comme un relecteur exigeant et signale les bugs probables, le code démodé et les noms mal écrits. Les deux lisent un fichier de règles à la racine du projet ; CLion les applique en direct.

---

## 🎨 clang-format : `.clang-format`

```yaml
---
Language: Cpp
BasedOnStyle: LLVM
Standard: Latest
ColumnLimit: 100
IndentWidth: 4
TabWidth: 4
UseTab: Never
BreakBeforeBraces: Allman
AccessModifierOffset: -4
NamespaceIndentation: None
PointerAlignment: Left
ReferenceAlignment: Left
AllowShortFunctionsOnASingleLine: Inline
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false
AllowShortLambdasOnASingleLine: Inline
AlwaysBreakTemplateDeclarations: Yes
BinPackArguments: false
BinPackParameters: false
BreakConstructorInitializers: BeforeComma
PackConstructorInitializers: Never
InsertNewlineAtEOF: true
SortIncludes: CaseSensitive
IncludeBlocks: Regroup
IncludeCategories:
  # 1. en-têtes du projet, 2. bibliothèques externes, 3. windows.h (toujours avant les
  # autres en-têtes Windows), 4. reste de l'API Windows, 5. bibliothèque standard
  - Regex: '^"'
    Priority: 1
  - Regex: '^<(saucer|glaze|cmrc|doctest|sqlite3)[/.]'
    Priority: 2
  - Regex: '^<windows\.h>'
    Priority: 3
  - Regex: '^<[A-Za-z0-9_]+\.h>'
    Priority: 4
  - Regex: '^<'
    Priority: 5
```

| Règle | Effet |
|---|---|
| `BreakBeforeBraces: Allman` | Accolade **sur sa propre ligne** |
| `ColumnLimit: 100` | Lignes de 100 caractères au plus |
| `IndentWidth: 4` | 4 espaces |
| `PointerAlignment: Left` | `int* p`, `const Book& b` |
| `BreakConstructorInitializers: BeforeComma` | La liste d'initialisation avec `,` en début de ligne |
| `IncludeBlocks: Regroup` + `IncludeCategories` | Les `#include` rangés en 5 groupes : projet, bibliothèques, `windows.h`, reste de Windows, STL |

```powershell
clang-format -i src\domain\book\book.cpp               # reformater un fichier
clang-format --dry-run --Werror src\**\*.cpp           # vérifier sans toucher
```

> [!tip] CLion
> **Settings › Editor › Code Style** : cocher *Enable ClangFormat*. `Ctrl+Alt+L` reformate avec les règles du projet.

### Désactiver localement

Quand l'ordre des `#include` est **imposé** (en-têtes Windows qui dépendent l'un de l'autre) :

```cpp
// clang-format off
#include <windows.h>
#include <objbase.h>
#include <saucer/modules/stable/webview2.hpp>
// clang-format on
```

---

## 🔬 clang-tidy : `.clang-tidy`

```yaml
---
# Analyse statique. Lancée par le preset CMake « tidy » et par l'éditeur à la volée.
Checks: >
  -*,
  bugprone-*,
  cppcoreguidelines-*,
  modernize-*,
  performance-*,
  misc-const-correctness,
  misc-unused-parameters,
  readability-container-size-empty,
  readability-identifier-naming,
  readability-redundant-string-cstr,
  -bugprone-easily-swappable-parameters,
  -cppcoreguidelines-avoid-const-or-ref-data-members,
  -cppcoreguidelines-avoid-magic-numbers,
  -cppcoreguidelines-pro-bounds-constant-array-index,
  -cppcoreguidelines-pro-bounds-pointer-arithmetic,
  -cppcoreguidelines-pro-type-vararg,
  -modernize-use-designated-initializers,
  -modernize-use-trailing-return-type,
  -modernize-use-nodiscard

WarningsAsErrors: '*'
HeaderFilterRegex: '.*[/\\](src|tests)[/\\].*'
FormatStyle: file

CheckOptions:
  # Types en PascalCase, fonctions et variables en camelCase, membres privés suivis de « _ ».
  readability-identifier-naming.ClassCase: CamelCase
  readability-identifier-naming.StructCase: CamelCase
  readability-identifier-naming.EnumCase: CamelCase
  readability-identifier-naming.EnumConstantCase: CamelCase
  readability-identifier-naming.TypeAliasCase: CamelCase
  readability-identifier-naming.FunctionCase: camelBack
  readability-identifier-naming.MethodCase: camelBack
  readability-identifier-naming.VariableCase: camelBack
  readability-identifier-naming.ParameterCase: camelBack
  readability-identifier-naming.MemberCase: camelBack
  readability-identifier-naming.PrivateMemberSuffix: '_'
  readability-identifier-naming.NamespaceCase: lower_case
  readability-identifier-naming.GlobalConstantCase: CamelCase
  readability-identifier-naming.StaticConstantCase: CamelCase
  readability-identifier-naming.ClassConstantCase: CamelCase
  readability-identifier-naming.ConstexprVariableCase: CamelCase
  readability-identifier-naming.TemplateParameterCase: CamelCase
  # Points d'entrée imposés par le système et macros de doctest.
  readability-identifier-naming.FunctionIgnoredRegexp: '^(wWinMain|main|DOCTEST_.*)$'
  misc-const-correctness.AnalyzeValues: true
  cppcoreguidelines-special-member-functions.AllowSoleDefaultDtor: true
  cppcoreguidelines-special-member-functions.AllowMissingMoveFunctionsWhenCopyIsDeleted: true
  cppcoreguidelines-macro-usage.AllowedRegexp: '^(SQLITE_|DOCTEST_|CMRC_|NOMINMAX|WIN32_).*'
```

| Famille | Ce qu'elle trouve |
|---|---|
| `bugprone-*` | Bugs probables : `catch` vide, conversions suspectes, `move` puis utilisation |
| `cppcoreguidelines-*` | Les règles des *C++ Core Guidelines* : pas de `new`, pas de cast C, membres initialisés |
| `modernize-*` | Code démodé : `NULL` → `nullptr`, boucle d'indices → `for` sur la collection |
| `performance-*` | Copies inutiles, `std::move` oublié |
| `misc-const-correctness` | Variables qui pourraient être `const` |
| `readability-identifier-naming` | Les conventions de nommage (voir [[01 Conventions de code C++]]) |

Les lignes `-xxx` retirent les règles trop bruyantes pour ce projet. `WarningsAsErrors: '*'` : tout est bloquant.

### Lancer

```powershell
cmake --preset tidy
cmake --build --preset tidy      # clang-tidy passe sur chaque fichier compilé
```

### `NOLINT` : une exception justifiée

```cpp
catch (...) // NOLINT(bugprone-empty-catch) : voulu, une tâche ne doit pas tuer le fil
{
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
return std::string{reinterpret_cast<const char*>(bytes), static_cast<std::size_t>(size)};
```

> [!warning] Toujours le nom de la règle et une raison
> Un `// NOLINT` nu fait taire **toutes** les règles sur la ligne et ne dit pas pourquoi. Écris `NOLINT(nom-de-la-règle)` suivi de la raison.

---

## 🔗 Liens

- [[01 Conventions de code C++]] — les règles de nommage expliquées
- [[08 Script de vérification avant commit]] — tout lancer d'un coup
