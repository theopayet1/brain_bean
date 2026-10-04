---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - sujet/memoire
  - sujet/debug
  - statut/a-jour
aliases:
  - Comportement indéfini
  - Undefined behavior
  - Référence pendante
cree: 2026-10-04
maj: 2026-10-04
---

# Durée de vie et pièges

> [!abstract] En une phrase
> Le bug mémoire typique du C++ moderne n'est plus la fuite, c'est la **référence pendante** : on garde une référence, un pointeur ou une vue vers un objet **déjà détruit**. Le programme a alors un **comportement indéfini** : il peut marcher, planter, ou corrompre des données. On l'évite avec quelques règles, et on le détecte avec **AddressSanitizer**.

---

## 💀 Comportement indéfini (*undefined behavior*, UB)

> [!info] Définition
> Un **comportement indéfini** est une situation où la norme C++ ne dit rien de ce qui se passe. Le compilateur suppose qu'elle **n'arrive jamais** : le résultat peut changer d'une compilation à l'autre, ou n'apparaître qu'en release.

Les plus courants :

| UB | Exemple |
|---|---|
| Lire un objet détruit | Référence / `string_view` vers une variable locale disparue |
| Sortir d'un tableau | `books[books.size()]`, `books.front()` sur une liste vide |
| Déréférencer `nullptr` | `*ptr` sans tester |
| Lire `*opt` sur un `optional` vide | `*books.get(id)` sans tester |
| Variable non initialisée | `int count; count++;` |
| Dépassement d'entier signé | `INT_MAX + 1` |
| Deux fils qui modifient la même donnée sans verrou | *data race*, voir [[05 Threads et file de tâches]] |

---

## 🪝 Les références pendantes

### Renvoyer une référence vers un local

```cpp
const std::string& title()
{
    std::string t = "Dune";
    return t;            // ❌ t meurt ici
}
```

✅ Renvoyer **par valeur** : `std::string title()`.

### Un `string_view` sur un temporaire

```cpp
std::string_view v = std::string{"temporaire"};   // ❌ la string meurt à la fin de la ligne
std::string_view w = book.title;                   // ✅ tant que book vit
```

### Une lambda qui survit à ce qu'elle capture

```cpp
void start(Worker& worker)
{
    std::string message = "...";
    worker.post([&message] { use(message); });   // ❌ exécutée plus tard : message est mort
    worker.post([message] { use(message); });    // ✅ copie
}
```

### Un objet détruit avant ceux qui l'utilisent

```cpp
BookService* service;
{
    FakeBooks books;
    service = new BookService{books, clock};   // le service garde une référence à books
}                                              // ❌ books est détruit, le service pointe dans le vide
```

✅ Déclarer les objets **dans l'ordre des dépendances** dans une même portée : ce qui est utilisé avant ce qui utilise. Voir [[06 La racine de composition]].

---

## 📏 Les règles qui évitent 95 % des problèmes

| Règle | Pourquoi |
|---|---|
| Renvoyer par **valeur** | Pas de référence vers un local |
| `string_view` et `T&` en **paramètre** seulement | L'appelant garantit que l'objet vit pendant l'appel |
| Les membres `T&` pointent vers des objets **plus vieux** que l'objet | Ordre de déclaration dans `main` |
| Lambda **gardée** → captures **par valeur** | Elle s'exécutera plus tard |
| Tester un `optional` / un pointeur avant de lire | Pas de lecture du vide |
| `.at(i)` si tu n'es pas sûr de l'indice | Exception au lieu d'UB |

---

## 🔍 Les détecter

| Outil | Ce qu'il trouve | Comment |
|---|---|---|
| **AddressSanitizer** (ASan) | Lecture d'objet détruit, débordement de tableau, fuite | `/fsanitize=address` (MSVC), `-fsanitize=address` (GCC/Clang), voir [[07 Déboguer et sanitizers]] |
| **UndefinedBehaviorSanitizer** (GCC/Clang) | Dépassements d'entier, `nullptr`, conversions | `-fsanitize=undefined` |
| **Avertissements** `/W4` | Variables non initialisées, conversions | [[05 Options de compilation et avertissements]] |
| **clang-tidy** | Beaucoup de motifs dangereux (`bugprone-*`) | [[06 clang-format et clang-tidy]] |

> [!tip] Lancer les tests en debug avec ASan
> C'est la configuration `debug` de [[03 CMakePresets]]. Un test qui passe en release mais plante sous ASan **a un vrai bug**.

---

## 🔗 Liens

- [[07 Déboguer et sanitizers]] — mettre en place ASan
- [[00 C++ moderne]] — la suite
