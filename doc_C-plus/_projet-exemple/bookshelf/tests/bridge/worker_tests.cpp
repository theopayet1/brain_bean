#include "bridge/worker.hpp"

#include <doctest/doctest.h>

#include <atomic>
#include <vector>

TEST_CASE("le fil de travail exécute tout, dans l'ordre, avant de s'arrêter")
{
    std::vector<int> done;
    {
        bookshelf::bridge::Worker worker;
        for (int i = 0; i < 100; ++i)
        {
            worker.post([&done, i] { done.push_back(i); });
        }
    } // le destructeur attend la fin des tâches

    REQUIRE(done.size() == 100);
    for (int i = 0; i < 100; ++i)
    {
        CHECK(done[static_cast<std::size_t>(i)] == i);
    }
}
