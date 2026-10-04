---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - sujet/memoire
  - statut/a-jour
aliases:
  - unique_ptr
  - shared_ptr
  - Smart pointers
cree: 2026-10-04
maj: 2026-10-04
---

# Pointeurs intelligents

> [!abstract] En une phrase
> Un **pointeur intelligent** est un pointeur qui **libère tout seul** ce qu'il pointe. **`std::unique_ptr`** : un seul propriétaire, le cas normal. **`std::shared_ptr`** : plusieurs propriétaires, l'objet meurt avec le dernier. Les deux remplacent `new` et `delete`.

---

## 1️⃣ `std::unique_ptr`

```cpp
#include <memory>

auto connection = std::make_unique<Connection>(path);   // 👈 crée sur le tas
connection->execute("PRAGMA foreign_keys = ON;");       // s'utilise comme un pointeur
// détruit automatiquement à la fin du bloc
```

- **Non copiable** : il n'y a qu'un propriétaire.
- **Déplaçable** : on peut transmettre la propriété avec `std::move`.

```cpp
std::unique_ptr<IBooks> books = std::make_unique<BooksSqlite>(connection);
std::unique_ptr<IBooks> other = books;              // ❌ erreur : pas de copie
std::unique_ptr<IBooks> owner = std::move(books);   // ✅ books est maintenant vide
```

---

## 🔌 Supprimeur personnalisé : RAII sur une API en C

Les bibliothèques en C (SQLite, Windows) donnent des pointeurs à libérer avec **leur** fonction. On l'indique au `unique_ptr` :

```cpp
struct sqlite3;   // déclaration anticipée : pas besoin d'inclure <sqlite3.h> dans le .hpp

class Connection
{
private:
    struct Close
    {
        void operator()(sqlite3* db) const noexcept;   // 👈 appelé à la destruction
    };

    std::unique_ptr<sqlite3, Close> db_;
};

// connection.cpp
void Connection::Close::operator()(sqlite3* db) const noexcept
{
    sqlite3_close_v2(db);
}
```

| Code | Pourquoi |
|---|---|
| `struct Close { void operator()(...) }` | Un « objet-fonction » qui sait libérer un `sqlite3*` |
| `std::unique_ptr<sqlite3, Close>` | Le `unique_ptr` appelle `Close` au lieu de `delete` |
| Pas de destructeur à écrire dans `Connection` | Celui généré détruit `db_`, qui ferme la base. **Règle de zéro** ([[04 Copie et déplacement]]) |

Récupérer le pointeur brut pour appeler l'API : `db_.get()`.

---

## 👥 `std::shared_ptr`

```cpp
auto app = saucer::application::init({.id = "bookshelf"});   // renvoie un shared_ptr
saucer::webview window{{.application = app}};                // la fenêtre en garde une copie
```

L'objet est détruit quand le **dernier** `shared_ptr` disparaît.

> [!warning] Ne pas en mettre partout
> `shared_ptr` coûte un compteur partagé et rend la durée de vie floue (« qui le garde en vie ? »). Utilise-le seulement quand la propriété est **vraiment partagée**, typiquement entre plusieurs fils ou avec une bibliothèque qui l'impose.

Exemple légitime : une promesse partagée avec un rappel qui peut arriver après qu'on a abandonné.

```cpp
auto promise = std::make_shared<std::promise<bool>>();
startAsyncWork([promise](bool ok) { promise->set_value(ok); });   // le rappel garde la promesse en vie
```

---

## 📋 Résumé

| Je veux… | J'utilise |
|---|---|
| Posséder seul un objet sur le tas | `std::unique_ptr<T>` + `std::make_unique<T>(...)` |
| Posséder un objet d'une API C | `std::unique_ptr<T, Supprimeur>` |
| Partager la propriété | `std::shared_ptr<T>` + `std::make_shared<T>(...)` |
| Juste **utiliser** un objet possédé ailleurs | `T&` ou `T*` |
| Une liste d'objets | `std::vector<T>` (pas de pointeur du tout) |

---

## 🔗 Liens

- [[02 RAII]] — le principe derrière
- [[01 SQLite — enveloppe RAII]] — l'exemple complet
- [[04 Copie et déplacement]] — la suite
