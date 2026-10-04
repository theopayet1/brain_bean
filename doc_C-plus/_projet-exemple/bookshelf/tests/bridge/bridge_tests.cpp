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
