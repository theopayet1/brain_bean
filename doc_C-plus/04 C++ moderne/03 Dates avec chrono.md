---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - statut/a-jour
aliases:
  - std::chrono
  - Dates C++
cree: 2026-10-04
maj: 2026-10-04
---

# Dates avec `chrono`

> [!abstract] En une phrase
> Depuis C++20, `<chrono>` gère les **dates de calendrier** (`year_month_day`) et les **instants** (`system_clock::time_point`) sans bibliothèque externe. On stocke les dates en **texte ISO** (`2026-10-04`) dans la base et le JSON, et on calcule avec `chrono` dans le domaine.

---

## 📅 Une date

```cpp
#include <chrono>

using namespace std::chrono;
using Date = std::chrono::year_month_day;

Date d = year{2026} / 10 / 4;        // 4 octobre 2026
Date e = 2026y / October / 4d;       // même chose, avec les littéraux

bool valid = (year{2026} / 2 / 30).ok();   // false : le 30 février n'existe pas
int y = static_cast<int>(d.year());
unsigned m = static_cast<unsigned>(d.month());
unsigned day = static_cast<unsigned>(d.day());
```

## ⏱️ Aujourd'hui

```cpp
const auto now = std::chrono::system_clock::now();               // un instant (UTC)
const Date today{std::chrono::floor<std::chrono::days>(now)};    // la date UTC
```

> [!warning] Fuseau horaire
> `system_clock` est en **UTC**. Pour la date **locale** (en France à 0 h 30, la date UTC est encore la veille) : `std::chrono::current_zone()->to_local(now)` (C++20, disponible sous MSVC). On met ce calcul derrière une interface `IClock`, voir plus bas.

## ➕ Calculer

```cpp
sys_days start = sys_days{year{2026} / 10 / 4};
sys_days later = start + days{30};          // 30 jours plus tard
Date result{later};

auto between = (sys_days{b} - sys_days{a}).count();   // nombre de jours entre a et b

Date nextMonth = d + months{1};             // ⚠️ le 31 janvier + 1 mois → 31 février : vérifier ok()
```

`sys_days` = un nombre de jours depuis 1970 : c'est la forme pour **compter**. `year_month_day` = la forme pour **lire** année / mois / jour.

---

## 📝 Texte ISO aller-retour

`src/domain/common/dates.cpp` :

```cpp
#include "domain/common/dates.hpp"

#include <charconv>
#include <format>

namespace bookshelf::domain
{

std::string formatIso(Date date)
{
    return std::format("{:%F}", std::chrono::sys_days{date});   // « 2026-10-04 »
}

namespace
{

// Lit exactement `text.size()` chiffres. Faux s'il y a autre chose.
bool readNumber(std::string_view text, int& value)
{
    const auto* end = text.data() + text.size();
    const auto [stop, error] = std::from_chars(text.data(), end, value);
    return error == std::errc{} && stop == end;
}

} // namespace

std::optional<Date> parseIso(std::string_view text)
{
    // AAAA-MM-JJ : 10 caractères, des tirets aux positions 4 et 7.
    if (text.size() != 10 || text[4] != '-' || text[7] != '-')
    {
        return std::nullopt;
    }
    int year = 0;
    int month = 0;
    int day = 0;
    if (!readNumber(text.substr(0, 4), year) || !readNumber(text.substr(5, 2), month) ||
        !readNumber(text.substr(8, 2), day))
    {
        return std::nullopt;
    }
    const Date date{std::chrono::year{year},
                    std::chrono::month{static_cast<unsigned>(month)},
                    std::chrono::day{static_cast<unsigned>(day)}};
    // ok() refuse le 30 février, le mois 13…
    if (!date.ok())
    {
        return std::nullopt;
    }
    return date;
}

} // namespace bookshelf::domain
```

| Code | Pourquoi |
|---|---|
| `{:%F}` | Format ISO `AAAA-MM-JJ` |
| `std::from_chars` | Lit un nombre **sans exception ni allocation**, et dit où il s'est arrêté |
| `stop == end` | Refuse `"1a"` : tout le texte doit être un nombre |
| `date.ok()` | Refuse les dates qui n'existent pas |

> [!info] Et `std::chrono::parse` ?
> C'est la fonction standard pour lire une date, mais elle n'existe qu'à partir de GCC 14 et dans MSVC récent. La version à la main ci-dessus marche partout et refuse plus strictement les saisies bizarres.

> [!tip] Pourquoi ISO dans la base
> `2026-10-04` se **trie** correctement comme du texte (`ORDER BY added_on`), se lit à l'œil, et c'est ce que JavaScript comprend (`new Date("2026-10-04")`).

---

## 🕰️ Une horloge qu'on peut fixer dans les tests

Un calcul qui dépend de « aujourd'hui » donne un résultat différent chaque jour : impossible à tester. On cache l'horloge derrière une **interface** :

```cpp
class IClock
{
public:
    virtual ~IClock() = default;
    [[nodiscard]] virtual std::chrono::year_month_day today() const = 0;
};
```

La vraie lit `system_clock` ; celle des tests renvoie une date fixe. Voir [[02 Tester un service avec des faux]].

---

## 🔗 Liens

- [[03 Les ports (interfaces)]] — pourquoi `IClock` est un port
- [[04 Algorithmes et ranges]] — la suite
