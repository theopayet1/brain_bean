---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - statut/a-jour
aliases:
  - constexpr
  - noexcept
  - nodiscard
cree: 2026-10-04
maj: 2026-10-04
---

# `const`, `constexpr`, `noexcept` et `[[nodiscard]]`

> [!abstract] En une phrase
> Quatre **promesses** faites au compilateur : `const` « je ne modifie pas », `constexpr` « calculable à la compilation », `noexcept` « je ne lève jamais d'exception », `[[nodiscard]]` « ignorer mon résultat est une erreur ». Le compilateur les **vérifie** et avertit quand on les trahit.

---

## 🔒 `const`

| Où | Sens |
|---|---|
| `const int x = 3;` | La variable ne change pas |
| `void f(const Book& b)` | `f` ne modifie pas le livre reçu |
| `int code() const;` | La méthode ne modifie pas l'objet |
| `const IClock& clock_;` | Le service n'appelle que des méthodes `const` de l'horloge |

> [!tip] « const par défaut »
> Mets `const` partout où c'est possible. clang-tidy le signale quand tu oublies (`misc-const-correctness`, voir [[06 clang-format et clang-tidy]]).

---

## ⚙️ `constexpr`

Une valeur ou une fonction **calculée pendant la compilation** :

```cpp
inline constexpr std::size_t MaxTitleLength = 200;
inline constexpr std::string_view BookNotFound = "book.not_found";

constexpr int square(int x) { return x * x; }
static_assert(square(4) == 16);   // vérifié à la compilation
```

| Écriture | Où |
|---|---|
| `inline constexpr T X = ...;` | Constante dans un **`.hpp`** (`inline` évite les doublons à l'édition de liens) |
| `constexpr T X = ...;` | Constante dans un `.cpp` ou une fonction |
| `static constexpr auto Table = std::to_array(...)` | Table constante dans une fonction |

---

## 🚫 `noexcept`

```cpp
[[nodiscard]] std::string handle(std::string_view message) noexcept;
std::int64_t lastInsertId() const noexcept;
```

- Si une exception sort quand même d'une fonction `noexcept`, le programme **s'arrête**. On ne le met donc que si c'est **vrai** (la fonction attrape tout, ou ne peut pas échouer).
- Les **destructeurs** sont `noexcept` par défaut.
- Un constructeur de déplacement `noexcept` permet à `std::vector` de déplacer au lieu de copier.

---

## ⚠️ `[[nodiscard]]`

```cpp
[[nodiscard]] Result<Id<Book>> add(NewBook request);

service.add(request);              // ⚠️ avertissement (erreur avec /WX) : l'échec est ignoré
auto id = service.add(request);    // ✅
(void)books.create(book);          // ✅ ignorer exprès, visiblement (dans un test)
```

À mettre sur : les fonctions qui renvoient un `Result`, un `optional`, un `bool` de succès, ou qui **calculent** quelque chose sans effet de bord.

---

## 🔗 Liens

- [[05 Options de compilation et avertissements]] — transformer ces avertissements en erreurs
- [[00 Construire un projet C++]] — la suite
