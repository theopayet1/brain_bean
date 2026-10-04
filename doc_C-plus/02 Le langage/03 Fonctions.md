---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - Fonctions C++
  - Passage de paramètres
cree: 2026-10-04
maj: 2026-10-04
---

# Fonctions

> [!abstract] En une phrase
> Une fonction a un **type de retour**, un **nom** et des **paramètres**. La vraie question en C++ est **comment passer les paramètres** : par copie, par référence, ou par référence constante. Ce choix change la vitesse et ce que la fonction a le droit de modifier.

---

## 🧩 Anatomie

```cpp
//  retour   nom         paramètres
    int      add        (int a, int b)
{
    return a + b;
}

void sayHello()            // void = ne renvoie rien
{
    std::println("Bonjour");
}
```

---

## 📦 Les trois façons de passer un paramètre

```cpp
void byValue(std::string text);            // reçoit une COPIE
void byReference(std::string& text);       // reçoit l'ORIGINAL, peut le modifier
void byConstRef(const std::string& text);  // reçoit l'original, NE PEUT PAS le modifier
```

> [!example] Le document
> - **Par valeur** : on te donne une **photocopie**. Tu peux griffonner dessus, l'original ne bouge pas. Ça coûte une photocopie.
> - **Par référence** : on te donne **l'original**. Tes griffonnages restent.
> - **Par référence constante** : on te montre l'original **sous une vitre**. Tu lis, sans photocopie, sans pouvoir écrire.

| Type du paramètre | Comment le passer |
|---|---|
| Petit : `int`, `bool`, `double`, `Id<Book>`, `std::string_view` | **Par valeur** |
| Gros, juste lu : `std::string`, `std::vector`, une struct | **`const&`** |
| La fonction doit le modifier pour l'appelant | **`&`** (rare : préfère renvoyer une valeur) |
| La fonction en garde une copie (le stocke) | **Par valeur + `std::move`**, voir [[04 Copie et déplacement]] |

```cpp
// Lu seulement : const&
bool contains(const std::vector<Book>& books, std::string_view title);

// Gardé dans l'objet : par valeur puis move
BookService::add(NewBook request)   // request est une copie à nous
{
    auto book = validate({.title = std::move(request.title), /* ... */});
}
```

---

## 🔁 Renvoyer des valeurs

Renvoyer un objet, même gros, **ne coûte rien** : le compilateur le construit directement à destination.

```cpp
std::vector<Book> list(std::string_view search);   // ✅ renvoyer par valeur
```

Pour renvoyer « peut-être rien » : `std::optional` ([[09 enum, optional et variant]]). Pour renvoyer « une valeur ou une erreur » : `std::expected` ([[01 Gérer les erreurs (expected et exceptions)]]).

---

## 🎚️ Surcharge et valeurs par défaut

Plusieurs fonctions peuvent porter **le même nom** si leurs paramètres diffèrent :

```cpp
Statement& bind(std::int64_t value);
Statement& bind(std::string_view value);
Statement& bind(std::nullopt_t);
```

Paramètre par défaut :

```cpp
std::unexpected<Error> fail(std::string_view code, std::string detail = {});

fail(errors::BookNotFound);                 // detail vaut ""
fail(errors::BookNotFound, "id = 42");
```

---

## 🏷️ `[[nodiscard]]`

```cpp
[[nodiscard]] bool remove(Id<Book> id);

books.remove(id);   // ⚠️ avertissement : tu ignores le résultat
```

À mettre sur toute fonction dont **ignorer le résultat est une erreur** (succès / échec, valeur calculée). Voir [[06 const, constexpr, noexcept et nodiscard]].

---

## 🧾 Déclaration et définition

- **Déclaration** : la signature seule, avec `;`. Va dans le `.hpp`.
- **Définition** : la signature et le corps `{ }`. Va dans le `.cpp`.

```cpp
// book.hpp
[[nodiscard]] Result<Book> validate(Book book);

// book.cpp
Result<Book> validate(Book book)
{
    // ...
}
```

Voir [[08 Fichiers hpp et cpp]].

---

## 🔗 Liens

- [[04 Chaînes de caractères]] — la suite
- [[06 Références et pointeurs]] — ce qu'est vraiment une référence
