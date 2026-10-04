---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - Types C++
cree: 2026-10-04
maj: 2026-10-04
---

# Variables et types

> [!abstract] En une phrase
> En C++, **chaque variable a un type fixé à la compilation** : un entier reste un entier. Le compilateur s'en sert pour vérifier ton code avant même qu'il tourne.

---

## 🧩 Les types de base

```cpp
int count = 42;              // entier
double price = 9.99;         // nombre à virgule
bool read = true;            // vrai / faux
char letter = 'A';           // UN caractère (guillemets simples)
std::string title = "Dune";  // du texte (guillemets doubles), #include <string>
```

| Type | Contient | Piège |
|---|---|---|
| `int` | Entier (au moins ±2 milliards) | Déborde sans prévenir au-delà |
| `std::int64_t` | Entier sur 64 bits, **taille garantie** (`#include <cstdint>`) | — |
| `std::size_t` | Taille, position dans une liste (jamais négatif) | Soustraire deux `size_t` peut donner un nombre énorme |
| `double` | Nombre à virgule | **Jamais pour de l'argent** : `0.1 + 0.2 != 0.3`. Utiliser des centimes entiers, voir [[02 Types forts]] |
| `bool` | `true` / `false` | — |
| `char` | Un octet | Un accent UTF-8 prend **plusieurs** `char` |

> [!tip] Entiers de taille fixe
> Pour un identifiant de base de données, utilise `std::int64_t` : c'est exactement ce que SQLite stocke ([[03 Un dépôt SQLite]]).

---

## 🪄 Initialiser

Toujours donner une valeur. Une variable non initialisée contient **n'importe quoi**.

```cpp
int a = 0;      // classique
int b{0};       // accolades : refuse les conversions qui perdent de l'info
int c{};        // {} vide = valeur « zéro » du type (0, false, "")
int d;          // ❌ valeur indéterminée : à ne jamais faire
```

```cpp
int x{3.7};     // ❌ erreur de compilation : 3.7 ne tient pas dans un int
int y = 3.7;    // ⚠️ compile, y vaut 3 (le compilateur avertit)
```

---

## 🤖 `auto` : laisser le compilateur déduire

```cpp
auto count = 42;                  // int
auto title = std::string{"Dune"}; // std::string
auto books = service.list("");    // le type renvoyé par list(), sans le réécrire
```

> [!warning] `auto` avec un texte entre guillemets
> `auto t = "Dune";` donne un `const char*`, **pas** un `std::string`. Écrire `std::string t = "Dune";`.

---

## 🔒 `const` : « ne changera pas »

```cpp
const int maxBooks = 500;
maxBooks = 600;   // ❌ erreur de compilation
```

Règle de cette doc : **tout ce qui ne change pas est `const`**. Ça documente le code et le compilateur l'interdit si tu te trompes. Pour les constantes connues à la compilation, `constexpr`, voir [[06 const, constexpr, noexcept et nodiscard]].

---

## 🔄 Convertir

```cpp
double average = 4.6;
int rounded = static_cast<int>(average);  // 4 : conversion EXPLICITE, voulue
```

| Écriture | À utiliser ? |
|---|---|
| `static_cast<int>(x)` | ✅ Oui : visible, cherchable |
| `(int)x` | ❌ Non : style C, cache des conversions dangereuses |
| `std::stoi("42")` | Texte → entier (lève une exception si ce n'est pas un nombre) |
| `std::to_string(42)` | Entier → texte |

---

## 📏 Portée

Une variable vit **jusqu'à l'accolade fermante** du bloc où elle est déclarée.

```cpp
{
    int x = 1;
}   // x n'existe plus ici
```

C'est la base du [[02 RAII|RAII]] : quand un objet sort de sa portée, il est détruit automatiquement.

---

## 🔗 Liens

- [[02 Conditions et boucles]] — la suite
- [[02 Types forts]] — des types qui empêchent de mélanger les choux et les carottes
