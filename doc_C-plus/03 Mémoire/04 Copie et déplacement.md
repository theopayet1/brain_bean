---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - sujet/memoire
  - statut/a-jour
aliases:
  - std::move
  - Règle de zéro
  - Move semantics
cree: 2026-10-04
maj: 2026-10-04
---

# Copie et déplacement

> [!abstract] En une phrase
> Copier un objet duplique tout son contenu ; le **déplacer** (`std::move`) **transfère** son contenu sans copie et laisse l'original vide. Pour tes propres classes, la bonne règle est la **règle de zéro** : n'écris ni copie, ni déplacement, ni destructeur — laisse les membres RAII s'en occuper — et **interdis la copie** (`= delete`) quand elle n'a pas de sens.

---

## 📋 Copier

```cpp
std::string a = "un long texte...";
std::string b = a;   // copie : b a son propre exemplaire, a est intact
```

## 🚚 Déplacer

```cpp
std::string a = "un long texte...";
std::string b = std::move(a);   // b récupère le texte, a est vide (mais valide)
```

> [!example] Le déménagement
> **Copier** un appartement : en construire un deuxième identique. **Déplacer** : donner les clés. Après, l'ancien propriétaire n'a plus rien, mais personne n'a rien reconstruit.

> [!warning] Après `std::move(x)`, ne lis plus `x`
> `x` est dans un état « valide mais non spécifié ». On peut lui réaffecter une valeur, rien d'autre.

---

## 🎯 Où déplacer

### Un paramètre qu'on garde

```cpp
Bridge::Bridge(BookService& books, Log log)
    : books_(books)
    , log_(std::move(log))   // 👈 log est une copie à nous : on la déplace dans le membre
{
}
```

### Un champ qu'on recopie ailleurs

```cpp
domain::Result<Id<Book>> BookService::add(NewBook request)
{
    auto book = domain::validate({
        .title = std::move(request.title),     // 👈 request ne sert plus après
        .author = std::move(request.author),
        .year = request.year,                  // un int : copier ne coûte rien
        .addedOn = clock_.today(),
    });
    // ...
}
```

### Une capture de lambda

```cpp
worker.post([message = std::move(message)]() mutable { /* ... */ });
```

> [!tip] Ne pas déplacer au `return`
> `return std::move(local);` empêche une optimisation. Écris `return local;` : le compilateur déplace (ou fait mieux) tout seul.

---

## 0️⃣ La règle de zéro

Si tous tes membres savent se copier / déplacer / détruire (des `std::string`, `std::vector`, `std::unique_ptr`…), **n'écris aucune** de ces fonctions : celles générées par le compilateur sont correctes.

```cpp
class Connection
{
    std::unique_ptr<sqlite3, Close> db_;   // sait se déplacer et se détruire
};
// ✅ Rien à écrire : Connection est déplaçable, non copiable (comme unique_ptr), et ferme la base.
```

## 5️⃣ La règle de cinq

Si tu écris **une** des cinq fonctions spéciales (destructeur, constructeur de copie, affectation par copie, constructeur de déplacement, affectation par déplacement), pense aux **cinq**. Le plus souvent, la réponse est : **interdire la copie**.

```cpp
class Transaction
{
public:
    ~Transaction();                                        // on a écrit un destructeur…
    Transaction(const Transaction&) = delete;              // …donc on décide pour la copie
    Transaction& operator=(const Transaction&) = delete;
    // le déplacement n'est alors plus généré : Transaction ne bouge pas, c'est voulu
};
```

---

## 🚫 `= delete` et `= default`

| Écriture | Sens |
|---|---|
| `T(const T&) = delete;` | Copie **interdite** (erreur de compilation si on essaie) |
| `virtual ~T() = default;` | Le compilateur écrit le destructeur, mais on le rend `virtual` |
| `friend auto operator<=>(T, T) = default;` | Comparaisons générées |

Les **interfaces** interdisent toujours la copie : voir [[12 Héritage et interfaces]].

---

## 🔗 Liens

- [[03 Fonctions]] — passer un paramètre par valeur pour le garder
- [[05 Durée de vie et pièges]] — la suite
