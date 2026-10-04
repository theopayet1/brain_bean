---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - Lambda C++
cree: 2026-10-04
maj: 2026-10-04
---

# Lambdas

> [!abstract] En une phrase
> Une **lambda** est une petite fonction écrite **sur place**, souvent passée à une autre fonction (« quand tu as fini, fais ça »). Les **captures** `[...]` disent quelles variables d'autour elle a le droit d'utiliser, et comment (copie ou référence).

---

## ✍️ La syntaxe

```cpp
//  captures  paramètres      corps
auto add =  [ ]       (int a, int b) { return a + b; };

int r = add(2, 3);   // 5
```

---

## 🎯 Les captures

```cpp
int minYear = 1950;

auto isRecent = [minYear](const Book& b) { return b.year.value_or(0) >= minYear; };  // copie
auto collect = [&found](const Book& b) { found.push_back(b); };                    // référence
```

| Capture | Sens | Danger |
|---|---|---|
| `[]` | Rien | — |
| `[x]` | **Copie** de `x` au moment de la création | — |
| `[&x]` | **Référence** à `x` | Si la lambda vit plus longtemps que `x` → référence pendante |
| `[this]` | L'objet courant (pour appeler ses méthodes) | Si l'objet meurt avant la lambda |
| `[x = std::move(y)]` | Crée `x` dans la lambda en **déplaçant** `y` | — |
| `[=]` / `[&]` | Tout, par copie / par référence | Cache ce qui est capturé : éviter |

> [!warning] Lambda gardée pour plus tard = captures par valeur
> Une lambda **exécutée tout de suite** (`std::ranges::sort(..., [&](...){})`) peut tout capturer par référence. Une lambda **gardée** (envoyée à un autre fil, appelée plus tard) doit capturer **par valeur** ce qui pourrait disparaître :
> ```cpp
> worker.post([&, message = std::move(message)]() mutable { /* ... */ });
> //            ^ les objets longs (pont, fenêtre) par référence
> //               ^ le message, qui va disparaître, DÉPLACÉ dans la lambda
> ```
> Voir [[08 Le fil de travail]].

---

## 🧰 Où on s'en sert

### Trier, chercher

```cpp
std::ranges::sort(books, [](const Book& a, const Book& b) { return a.title < b.title; });

auto it = std::ranges::find_if(books, [](const Book& b) { return !b.read; });
```

### Réagir à un événement

```cpp
window.on<saucer::web_event::navigate>(
    [](const saucer::navigation& navigation)
    {
        return isFrontendUrl(navigation.url()) ? saucer::policy::allow : saucer::policy::block;
    });
```

### Déclarer une table constante dans une fonction

```cpp
const auto isSpace = [](char c) { return c == ' ' || c == '\t'; };
```

---

## 📦 Garder une lambda : `std::function`

Le type exact d'une lambda est inconnu (seul le compilateur le connaît). Pour la **stocker** dans un membre :

```cpp
#include <functional>

class Bridge
{
public:
    using Log = std::function<void(std::string_view function, std::string_view detail)>;
    Bridge(BookService& books, Log log);
private:
    Log log_;
};
```

| Type | Quand |
|---|---|
| `std::function<R(Args...)>` | Garder une fonction copiable |
| `std::move_only_function<R(Args...)>` (C++23) | Garder une fonction qui contient des objets non copiables (un `unique_ptr`, un message déplacé) |
| `auto` / paramètre template | Juste la passer, sans la stocker |

### `mutable`

Par défaut, une lambda ne peut pas modifier ce qu'elle a **copié**. `mutable` l'autorise (utile pour `std::move` une capture).

---

## 🔗 Liens

- [[04 Algorithmes et ranges]] — les lambdas avec les algorithmes
- [[11 Templates]] — la suite
