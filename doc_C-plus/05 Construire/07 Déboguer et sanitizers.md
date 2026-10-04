---
tags:
  - projet/cpp
  - type/guide
  - techno/cpp
  - techno/msvc
  - techno/clion
  - sujet/debug
  - statut/a-jour
aliases:
  - Débogage C++
  - AddressSanitizer
cree: 2026-10-04
maj: 2026-10-04
---

# Déboguer et sanitizers

> [!abstract] En une phrase
> Trois outils : le **débogueur** (arrêter le programme à une ligne et regarder les variables), **AddressSanitizer** (le programme s'arrête **net** à la première erreur mémoire, avec la ligne fautive), et les **DevTools** de la webview (F12) pour l'interface. Plus le **journal** pour ce qui arrive chez l'utilisateur.

---

## 🐞 Le débogueur

Dans CLion (ou Visual Studio, VS Code) avec le preset **debug** :

| Action | Raccourci CLion |
|---|---|
| Poser un point d'arrêt | Clic dans la marge |
| Lancer en debug | `Shift+F9` |
| Ligne suivante | `F8` |
| Entrer dans la fonction | `F7` |
| Continuer jusqu'au prochain point d'arrêt | `F9` |
| Évaluer une expression | `Alt+F8` |

> [!tip] Déboguer un test
> Chaque exe de tests (`tests_domain`, `tests_application`…) est une cible : on le lance en debug comme l'application. Pour ne lancer qu'un test doctest : argument `--test-case="un livre sans titre*"`.

> [!info] Debug ou release
> En **release**, le compilateur réordonne et supprime du code : les variables affichées peuvent être fausses. On débogue en **debug**.

---

## 🧪 AddressSanitizer (ASan)

ASan ajoute des vérifications autour de chaque accès mémoire. Au premier accès interdit, le programme s'arrête et affiche **où** l'objet a été créé, détruit, puis lu.

Activé par le preset `debug` (`BOOKSHELF_ASAN=ON`), voir [[05 Options de compilation et avertissements]].

```text
==1234==ERROR: AddressSanitizer: heap-use-after-free on address 0x...
READ of size 8 at 0x... thread T0
    #0 in BookService::list book_service.cpp:22
freed by thread T0 here:
    #0 in std::vector<Book>::~vector
previously allocated by thread T0 here:
    ...
```

| Message | Sens |
|---|---|
| `heap-use-after-free` | Lecture d'un objet du tas **déjà libéré** (référence pendante) |
| `stack-use-after-return` | Référence vers une variable locale d'une fonction terminée |
| `heap-buffer-overflow` / `stack-buffer-overflow` | Lecture au-delà de la fin d'un tableau |
| `detected memory leaks` (GCC/Clang) | Mémoire jamais libérée |

> [!warning] Sous MSVC
> - Il faut le composant **C++ AddressSanitizer** (Visual Studio Installer).
> - `/INCREMENTAL:NO` est obligatoire, et les bibliothèques vcpkg n'étant pas compilées avec ASan, il faut `_DISABLE_VECTOR_ANNOTATION` et `_DISABLE_STRING_ANNOTATION`. Tout est déjà dans `CompilerOptions.cmake`.

---

## 🌐 Les DevTools de la webview

En **debug**, la fenêtre autorise les outils de développement :

```cpp
#ifdef NDEBUG
    window.set_dev_tools(false);
    window.set_context_menu(false);
#else
    window.set_dev_tools(true);
#endif
```

Clic droit › **Inspecter**, ou `F12` : console JavaScript, inspection du HTML/CSS, onglet réseau (qui doit rester **vide**, voir [[09 Sécuriser la webview]]).

Pour travailler l'interface **sans** le C++ : `npm run dev` et le navigateur, voir [[12 Travailler l'interface sans le C++]].

---

## 📓 Le journal

Ce qui arrive chez l'utilisateur, on ne le voit pas. Les erreurs techniques sont écrites dans `%LOCALAPPDATA%\Bookshelf\errors.log` par le pont ([[06 Le pont côté C++]]) :

```text
2026-10-04 08:12:44 UTC [listBooks] database is locked
```

Jamais de donnée personnelle ni de mot de passe dans ce fichier : seulement le **nom de la fonction** et le **détail technique**.

---

## 🔗 Liens

- [[05 Durée de vie et pièges]] — ce qu'ASan détecte
- [[08 Script de vérification avant commit]] — la suite
