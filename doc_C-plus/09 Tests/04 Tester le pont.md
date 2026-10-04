---
tags:
  - projet/cpp
  - type/guide
  - techno/doctest
  - techno/glaze
  - sujet/tests
  - sujet/ui
  - statut/a-jour
aliases:
  - Tester le bridge
cree: 2026-10-04
maj: 2026-10-04
---

# Tester le pont

> [!abstract] En une phrase
> Comme le pont reçoit un texte et renvoie un texte, on le teste **sans fenêtre** : on lui passe une demande JSON et on compare la réponse **caractère pour caractère**. C'est le test du **contrat** avec le TypeScript : s'il passe, l'interface recevra exactement ce qu'elle attend.

---

## 📄 `tests/bridge/bridge_tests.cpp`

```cpp
#include "bridge/bridge.hpp"
#include "support/fakes.hpp"

#include <doctest/doctest.h>

#include <string>

using namespace bookshelf;

namespace
{

// Un pont branché sur de faux livres : on teste la forme EXACTE des JSON.
struct Fixture
{
    tests::FakeBooks books;
    tests::FixedClock clock;
    application::BookService service{books, clock};
    std::string log;
    bridge::Bridge bridge{service,
                          [this](std::string_view function, std::string_view detail)
                          {
                              log += std::string{function} + ": " + std::string{detail};
                          }};
};

} // namespace

TEST_CASE("ajouter puis lister un livre")
{
    Fixture f;

    CHECK(
        f.bridge.handle(
            R"({"id":1,"function":"addBook","request":{"title":"Dune","author":"Frank Herbert","year":1965}})") ==
        R"({"id":1,"ok":true,"data":{"id":1}})");

    CHECK(
        f.bridge.handle(R"({"id":2,"function":"listBooks","request":{"search":""}})") ==
        R"({"id":2,"ok":true,"data":[{"id":1,"title":"Dune","author":"Frank Herbert","year":1965,"read":false,"addedOn":"2026-10-04"}]})");
}

TEST_CASE("une erreur métier devient une réponse lisible")
{
    Fixture f;

    CHECK(
        f.bridge.handle(R"({"id":3,"function":"addBook","request":{"title":"","author":""}})") ==
        R"({"id":3,"ok":false,"code":"book.title_missing","message":"Le titre est obligatoire."})");
}

TEST_CASE("une fonction inconnue ou un JSON cassé ne fait pas planter")
{
    Fixture f;

    CHECK(f.bridge.handle(R"({"id":4,"function":"deleteEverything","request":{}})")
              .find("bridge.unknown_function") != std::string::npos);
    CHECK(f.bridge.handle("pas du json").find("bridge.invalid_request") != std::string::npos);
    // Clé en trop : refusée, le frontend a un bug.
    CHECK(f.bridge.handle(R"({"id":5,"function":"removeBook","request":{"id":1,"oups":true}})")
              .find("bridge.invalid_request") != std::string::npos);
}
```

| Point | Pourquoi |
|---|---|
| `struct Fixture` | Prépare d'un coup faux dépôt, horloge, service et pont. Les membres sont construits **dans l'ordre** |
| `R"(...)"` | Chaîne brute : les `"` du JSON sans `\` |
| Comparaison **exacte** du JSON | Un champ renommé, un `null` en trop : le test casse, comme casserait l'interface |
| Les cas d'erreur : métier, fonction inconnue, JSON cassé, clé en trop | Le pont **ne lève jamais** et répond toujours proprement |

> [!tip] Quand un test du pont casse après un changement voulu
> C'est que le **contrat** a changé : mettre à jour le test **et** `types.ts` / `api.ts` dans le même commit.

---

## 🔗 Liens

- [[06 Le pont côté C++]] — le code testé
- [[05 Le pont C++ JavaScript — le protocole]] — le contrat
- [[00 Tutoriel — une app de bureau complète]] — la suite
