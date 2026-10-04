---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - sujet/compilation
  - statut/a-jour
aliases:
  - Qu'est-ce que le C++
cree: 2026-10-04
maj: 2026-10-04
---

# C++, c'est quoi ?

> [!abstract] En une phrase
> Le C++ est un langage **compilé** : ton texte est traduit **une fois pour toutes** en instructions machine, ce qui donne des programmes très rapides et sans dépendance, mais qui demande plus de rigueur (types, mémoire, erreurs) qu'un langage comme Python ou C#.

---

## 🧩 Compilé ou interprété

> [!info] Définition — compilateur
> Un **compilateur** est un programme qui lit ton code source et fabrique un **exécutable** (`.exe` sous Windows) que le processeur comprend directement.

| | C++ | C# / Java | Python / JavaScript |
|---|---|---|---|
| Traduit quand ? | **Avant** de lancer (compilation) | Avant, vers un code intermédiaire, puis à la volée | Pendant l'exécution |
| Ce qu'on livre | Un `.exe` autonome | Un `.dll` + le runtime .NET / JVM | Les sources + l'interpréteur |
| Vitesse | Maximale | Très bonne | Plus lente |
| Mémoire | **Gérée par toi** (avec de l'aide, voir [[02 RAII]]) | Ramasse-miettes (*garbage collector*) | Ramasse-miettes |
| Erreurs de type | Trouvées **à la compilation** | À la compilation | Souvent à l'exécution |

> [!example] La recette et le gâteau
> Le code source est une **recette**. Le compilateur est le **pâtissier** qui fait le gâteau une fois. Tu livres le **gâteau** (le `.exe`), pas la recette ni le pâtissier. Avec Python, tu livres la recette et un pâtissier qui cuisine à chaque fois.

---

## ⚡ Pourquoi choisir le C++

- **Rapide** : rien entre ton code et le processeur.
- **Autonome** : un seul `.exe`, sans runtime à installer (si on lie tout en statique, voir [[04 vcpkg — les dépendances]]).
- **Accès direct au système** : API Windows, fichiers, matériel.
- **Durable** : du C++ écrit il y a 20 ans compile encore.

## 😬 Ce qui est plus dur

- **La mémoire** : un objet détruit trop tôt, et le programme fait n'importe quoi. Le C++ moderne règle presque tout avec le [[02 RAII|RAII]] et les [[03 Pointeurs intelligents|pointeurs intelligents]].
- **La compilation** est plus lente et ses messages d'erreur sont longs. Voir [[02 Erreurs fréquentes C++]].
- **Le build** : il faut un outil pour dire quels fichiers compiler et avec quoi les lier, c'est **CMake** ([[01 CMake — les bases]]).

---

## 📅 Les versions du C++

Le langage évolue par **normes** publiées tous les 3 ans. On parle de « C++17 », « C++20 », « C++23 ».

| Norme | Ce qu'elle apporte d'important pour nous |
|---|---|
| C++11 | `auto`, lambdas, `std::unique_ptr`, `std::move` : le début du **C++ moderne** |
| C++17 | `std::optional`, `std::variant`, `std::string_view`, `std::filesystem` |
| C++20 | `std::format`, les *ranges*, `std::jthread`, `<=>`, `std::chrono` pour les dates |
| **C++23** | `std::expected`, `std::println`, `std::move_only_function` |

> [!tip] Ce que cette doc utilise
> **C++23**, partout. Décidé parce que `std::expected` (C++23) donne une façon propre de renvoyer une erreur, voir [[01 Gérer les erreurs (expected et exceptions)]], et que la bibliothèque d'interface utilisée ([[02 saucer — ouvrir une fenêtre|saucer]]) l'exige.

---

## 🧱 Le vocabulaire de base

| Mot | Sens |
|---|---|
| **Fichier source** (`.cpp`) | Le code qui fait des choses |
| **En-tête** (`.hpp`) | Les déclarations : « cette fonction existe, voilà sa signature ». Voir [[08 Fichiers hpp et cpp]] |
| **Compilateur** | `cl.exe` (MSVC, Microsoft), `g++` (GCC), `clang++` (Clang) |
| **Éditeur de liens** (*linker*) | Assemble les morceaux compilés en un seul `.exe` |
| **STL** / bibliothèque standard | Tout ce qui commence par `std::` : chaînes, listes, fichiers, dates… |
| **Bibliothèque** (*library*) | Du code déjà écrit qu'on réutilise (SQLite, glaze…) |

---

## 🔗 Liens

- [[02 Installer les outils C++]] — la suite : préparer son poste
- [[04 De la source à l'exe]] — ce que fait vraiment le compilateur
