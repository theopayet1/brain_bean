---
tags:
  - projet/cpp
  - type/guide
  - techno/cpp
  - techno/msvc
  - sujet/compilation
  - statut/a-jour
aliases:
  - Hello world C++
cree: 2026-10-04
maj: 2026-10-04
---

# Premier programme

> [!abstract] En une phrase
> Tout programme C++ commence par une fonction **`main()`**. On l'écrit dans un fichier `.cpp`, on le **compile** avec une commande, et on lance l'exécutable obtenu. À lire avant : [[02 Installer les outils C++]].

---

## 👋 Le code

`main.cpp` :

```cpp
#include <print>   // pour std::println
#include <string>

int main()
{
    std::string name = "monde";
    std::println("Bonjour, {} !", name);
    return 0;
}
```

| Code | Pourquoi |
|---|---|
| `#include <print>` | « Colle ici » les déclarations de `std::println`. Sans ça, le compilateur ne la connaît pas |
| `int main()` | Le **point d'entrée** : le système appelle cette fonction au lancement |
| `std::string name = "monde";` | Une variable de type texte. `std::` = elle vient de la bibliothèque standard |
| `std::println("…{}…", name)` | Affiche une ligne. `{}` est remplacé par `name` |
| `return 0;` | Code de sortie : `0` = tout s'est bien passé |

---

## 🔨 Compiler à la main

Dans le **Developer PowerShell** :

```powershell
cl /std:c++latest /EHsc /utf-8 main.cpp
.\main.exe
```

```text
Bonjour, monde !
```

| Option | Pourquoi |
|---|---|
| `/std:c++latest` | Active la dernière norme (C++23) |
| `/EHsc` | Active les exceptions C++ (toujours la mettre) |
| `/utf-8` | Les accents de tes chaînes restent corrects |

Avec GCC ou Clang (Linux, macOS) :

```bash
g++ -std=c++23 -Wall -Wextra main.cpp -o main
./main
```

> [!tip] On ne compile à la main que pour essayer
> Dès qu'il y a plus d'un fichier, on utilise **CMake** : voir [[01 CMake — les bases]].

---

## 🧪 Lire ce que tape l'utilisateur

```cpp
#include <iostream>
#include <print>
#include <string>

int main()
{
    std::println("Ton prénom ?");
    std::string name;
    std::getline(std::cin, name);   // 👈 lit toute la ligne tapée
    std::println("Salut {} !", name);
}
```

> [!info] `return 0` est facultatif dans `main`
> C'est la **seule** fonction où l'oublier est permis : elle renvoie alors `0`.

---

## ⚠️ Erreurs fréquentes

| Symptôme | Cause | Solution |
|---|---|---|
| `'println' n'est pas membre de 'std'` | Norme trop ancienne ou `#include <print>` oublié | `/std:c++latest` et l'include |
| `fatal error C1034: print: aucun chemin d'accès Include` | Terminal ordinaire, pas le Developer PowerShell | Voir [[02 Installer les outils C++]] |
| Les accents s'affichent `Ã©` | Source lue dans le mauvais encodage | `/utf-8`, et fichiers enregistrés en UTF-8 |
| `error C2143: syntax error: missing ';'` | Un `;` oublié à la ligne **d'avant** | Regarder la ligne au-dessus de celle indiquée |

---

## 🔗 Liens

- [[04 De la source à l'exe]] — ce qui se passe pendant `cl main.cpp`
- [[01 Variables et types]] — la suite dans le langage
