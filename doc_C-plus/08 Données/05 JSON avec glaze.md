---
tags:
  - projet/cpp
  - type/guide
  - techno/glaze
  - techno/cpp
  - statut/a-jour
aliases:
  - glaze
  - JSON C++
cree: 2026-10-04
maj: 2026-10-04
---

# JSON avec glaze

> [!abstract] En une phrase
> **glaze** lit et écrit du JSON **directement à partir de tes structs** : le nom de chaque membre devient la clé JSON, sans écrire une ligne de conversion. Il est très rapide, et c'est déjà une dépendance de saucer : une seule bibliothèque JSON dans tout le projet.

---

## ✍️ Écrire

```cpp
#include <glaze/json.hpp>

struct Book
{
    std::int64_t id = 0;
    std::string title;
    std::optional<int> year;
    std::vector<std::string> tags;
};

const Book dune{.id = 1, .title = "Dune", .year = 1965, .tags = {"sf"}};
auto json = glz::write_json(dune);          // std::expected<std::string, erreur>
std::println("{}", *json);
```

```json
{"id":1,"title":"Dune","year":1965,"tags":["sf"]}
```

Un `std::optional` vide est **omis** : `{"id":2,"title":"?","tags":[]}`.

## 📖 Lire

```cpp
Book book{};
auto error = glz::read_json(book, R"({"id":3,"title":"Fondation","tags":[]})");
if (error)
{
    // échec : JSON invalide ou mauvais type
}
```

> [!warning] `error` est vrai en cas d'**échec**
> `glz::read_json` renvoie une erreur ; `if (error)` veut dire « ça a raté ». C'est l'inverse d'un `expected`.

---

## 🔒 Lire strictement

```cpp
constexpr glz::opts Strict{.error_on_unknown_keys = true, .error_on_missing_keys = true};

auto error = glz::read<Strict>(book, json);
if (error)
{
    std::string why = glz::format_error(error, json);   // pour le journal
}
```

```text
1:32: unknown_key
   {"id":3,"title":"x","tags":[],"oups":1}
                                  ^
```

| Option | Effet |
|---|---|
| `error_on_unknown_keys` | Une clé qui n'est pas un membre = erreur. C'est déjà le comportement par défaut de glaze : on l'écrit pour que l'intention soit visible |
| `error_on_missing_keys` | Une clé absente = erreur, **sauf** pour les membres `std::optional`. Désactivé par défaut : c'est l'option qui rend la lecture vraiment stricte |

> [!tip] Ne pas mettre `format_error` dans la réponse à l'interface
> Le message cite un extrait du JSON reçu, qui peut contenir des données personnelles ou un mot de passe. Il va **au journal**, jamais à l'écran.

---

## 🧩 Types pris en charge

| C++ | JSON |
|---|---|
| `int`, `std::int64_t`, `double` | nombre |
| `bool` | `true` / `false` |
| `std::string` | texte |
| `std::optional<T>` | `T`, ou clé absente |
| `std::vector<T>` | tableau |
| `std::map<std::string, T>` | objet |
| struct simple (agrégat) | objet, une clé par membre |
| `glz::raw_json` | le JSON **brut**, non interprété (`.str`) |

### `glz::raw_json` : lire plus tard

```cpp
struct Envelope
{
    std::uint64_t id = 0;
    std::string function;
    glz::raw_json request;   // 👈 gardé tel quel
};
```

Le pont lit l'enveloppe, puis passe `request.str` à la fonction qui sait quelle forme attendre ([[06 Le pont côté C++]]).

---

## ⚠️ Pièges

| Symptôme | Cause | Solution |
|---|---|---|
| `declared using local type ..., is used but never defined` | Struct déclarée **dans une fonction** ou dans un `namespace { }` anonyme | La déclarer dans un namespace **nommé** (glaze a besoin d'une liaison externe) |
| Compilation très longue | glaze est un gros en-tête de templates | Ne l'inclure que dans les `.cpp` qui en ont besoin (le pont), jamais dans un `.hpp` partagé |
| Une clé JSON a changé de nom | Membre C++ renommé | Le nom du membre **est** le contrat : changer `types.ts` en même temps |

> [!note] Vérifié le 2026-10-04
> Tous les exemples de cette note ont été compilés et exécutés avec glaze et GCC 13 en C++23.

---

## 🔗 Liens

- [[06 Le pont côté C++]] — glaze au travail
- [[06 Base chiffrée (SQLCipher)]] — la suite
