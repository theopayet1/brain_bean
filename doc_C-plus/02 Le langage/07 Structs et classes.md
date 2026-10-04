---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - Classes C++
  - struct C++
cree: 2026-10-04
maj: 2026-10-04
---

# Structs et classes

> [!abstract] En une phrase
> Une **`struct`** regroupe des données publiques (un livre : titre, auteur, année) ; une **`class`** protège ses données (`private`) et n'offre que des méthodes. En C++ les deux mots font presque la même chose : la convention est **`struct` pour des données « bêtes », `class` pour un objet qui a des règles ou des ressources**.

---

## 📦 Une struct de données

```cpp
struct Book
{
    Id<Book> id;
    std::string title;
    std::string author;
    std::optional<int> year;   // peut être absent
    bool read = false;         // 👈 valeur par défaut
    Date addedOn{};
};
```

### L'initialisation désignée (C++20)

On nomme les champs, **dans l'ordre de déclaration** :

```cpp
Book dune{
    .title = "Dune",
    .author = "Frank Herbert",
    .year = 1965,
};
// id, read, addedOn gardent leur valeur par défaut
```

> [!tip] C'est la façon la plus lisible de construire une struct
> On voit tout de suite quel champ reçoit quoi. Les champs oubliés prennent leur valeur par défaut.

---

## 🏗️ Une classe avec des règles

```cpp
class BookService
{
public:                                            // 👈 ce que les autres peuvent appeler
    BookService(IBooks& books, const IClock& clock);

    [[nodiscard]] std::vector<Book> list(std::string_view search);
    [[nodiscard]] Result<Id<Book>> add(NewBook request);

private:                                           // 👈 ce que la classe seule voit
    IBooks& books_;
    const IClock& clock_;
};
```

| Mot | Sens |
|---|---|
| `public:` | Visible de partout |
| `private:` | Visible seulement dans la classe |
| `protected:` | Visible dans la classe et ses classes filles ([[12 Héritage et interfaces]]) |
| `books_` | Convention : les membres privés finissent par `_` ([[01 Conventions de code C++]]) |

---

## 🛠️ Le constructeur et sa liste d'initialisation

```cpp
BookService::BookService(IBooks& books, const IClock& clock)
    : books_(books)     // 👈 liste d'initialisation : les membres sont construits ICI
    , clock_(clock)
{
    // le corps : souvent vide
}
```

> [!warning] Les références et `const` DOIVENT passer par la liste
> Un membre `IBooks&` ne peut pas être « affecté » dans le corps `{}` : il doit être lié à la construction. La liste d'initialisation est donc obligatoire ici, et c'est la bonne habitude partout.

> [!info] Ordre de construction
> Les membres sont construits dans **l'ordre où ils sont déclarés dans la classe**, pas dans l'ordre de la liste. Et détruits dans l'ordre **inverse**. On s'en sert exprès : voir [[05 Threads et file de tâches]].

### `explicit`

Un constructeur à un seul paramètre doit être `explicit`, sinon le compilateur s'en sert pour des conversions cachées :

```cpp
class BooksSqlite
{
public:
    explicit BooksSqlite(Connection& connection);
};
```

---

## 🧽 Le destructeur

Appelé **automatiquement** quand l'objet meurt. On y libère ce que l'objet possède :

```cpp
class Transaction
{
public:
    explicit Transaction(Connection& connection);
    ~Transaction();   // 👈 annule la transaction si elle n'a pas été validée
};
```

C'est le cœur du [[02 RAII]].

---

## ⚖️ Comparer : `operator<=>`

```cpp
template <typename T>
struct Id
{
    std::int64_t value = 0;

    friend constexpr auto operator<=>(Id, Id) = default;   // 👈 ==, !=, <, >… générés
};
```

`= default` demande au compilateur d'écrire la comparaison champ par champ.

---

## 🧭 Méthodes `const`

```cpp
class Statement
{
public:
    [[nodiscard]] std::int64_t integer(int column) const;   // 👈 promet de ne rien modifier
};
```

Une méthode `const` peut être appelée sur un objet `const`. Mets `const` sur **toute méthode qui ne modifie pas l'objet**.

---

## 🔗 Liens

- [[08 Fichiers hpp et cpp]] — où écrire la classe et ses méthodes
- [[02 RAII]] — constructeur qui prend, destructeur qui rend
- [[04 Copie et déplacement]] — ce qui se passe quand on copie un objet
