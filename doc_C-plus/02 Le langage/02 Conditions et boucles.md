---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - if for while C++
cree: 2026-10-04
maj: 2026-10-04
---

# Conditions et boucles

> [!abstract] En une phrase
> `if` choisit, `switch` aiguille sur une valeur, `for` et `while` répètent. Le **`for` sur une collection** (`for (const auto& x : liste)`) est celui qu'on écrit 90 % du temps.

---

## 🔀 `if` / `else`

```cpp
if (book.title.empty())
{
    return fail(errors::BookTitleMissing);
}
else if (book.title.size() > MaxTitleLength)
{
    return fail(errors::BookTitleTooLong);
}
```

### `if` avec initialisation

On déclare une variable **qui ne vit que dans le `if`** :

```cpp
if (auto found = books.get(id); found)   // 👈 found n'existe que dans ce if/else
{
    std::println("{}", found->title);
}
```

| Opérateur | Sens |
|---|---|
| `==` / `!=` | égal / différent |
| `<` `<=` `>` `>=` | comparaisons |
| `&&` / `||` / `!` | et / ou / non |

> [!warning] `=` n'est pas `==`
> `if (x = 5)` **affecte** 5 à `x` et vaut toujours vrai. Les avertissements du compilateur ([[05 Options de compilation et avertissements]]) le signalent.

---

## 🎛️ `switch`

Pour aiguiller sur un entier ou un `enum` :

```cpp
switch (state)
{
case LockState::Locked:
    return "locked";
case LockState::Open:
    return "open";
}
```

> [!tip] Pas de `default` sur un `enum class`
> Sans `default`, le compilateur **avertit** si tu oublies une valeur quand tu en ajoutes une à l'`enum`. Voir [[09 enum, optional et variant]].

Dans un `switch` classique, chaque `case` doit finir par `return` ou `break`, sinon on « tombe » dans le suivant.

---

## 🔁 `for` sur une collection

```cpp
std::vector<Book> books = service.list("");

for (const Book& book : books)       // 👈 const& : lire sans copier
{
    std::println("{} — {}", book.title, book.author);
}
```

| Écriture | Quand |
|---|---|
| `for (const auto& b : books)` | **Lire** (le cas normal) |
| `for (auto& b : books)` | **Modifier** chaque élément |
| `for (auto b : books)` | Travailler sur une **copie** (rare) |

### Avec des paires (map)

```cpp
for (const auto& [id, book] : booksById)   // « décomposition » de la paire
{
    std::println("{} : {}", id, book.title);
}
```

---

## 🔢 `for` classique et `while`

```cpp
for (int i = 0; i < 10; ++i)
{
    std::println("{}", i);
}

while (statement.next())        // tant qu'il y a une ligne
{
    books.push_back(readBook(statement));
}

for (;;)                        // boucle infinie (sortie par return ou break)
{
    // ...
}
```

`++i` ajoute 1 à `i`. `break` sort de la boucle, `continue` passe au tour suivant.

---

## 🔗 Liens

- [[03 Fonctions]] — la suite
- [[04 Algorithmes et ranges]] — trier et chercher sans écrire de boucle
