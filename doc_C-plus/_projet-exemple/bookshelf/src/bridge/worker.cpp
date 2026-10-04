#include "bridge/worker.hpp"

#include <utility>

namespace bookshelf::bridge
{

Worker::Worker()
    : thread_([this] { loop(); })
{
}

Worker::~Worker()
{
    {
        const std::scoped_lock lock{mutex_};
        stopping_ = true;
    }
    wakeUp_.notify_one();
    // Le destructeur de std::jthread attend la fin du fil (join) tout seul.
}

void Worker::post(Task task)
{
    {
        const std::scoped_lock lock{mutex_};
        tasks_.push_back(std::move(task));
    }
    wakeUp_.notify_one();
}

void Worker::loop()
{
    for (;;)
    {
        Task task;
        {
            std::unique_lock lock{mutex_};
            // Dort jusqu'à ce qu'il y ait du travail ou qu'on demande l'arrêt.
            wakeUp_.wait(lock, [this] { return stopping_ || !tasks_.empty(); });
            if (tasks_.empty())
            {
                return; // arrêt demandé et plus rien à faire
            }
            task = std::move(tasks_.front());
            tasks_.pop_front();
        }
        // Exécutée HORS du verrou : post() reste possible pendant une tâche longue.
        try
        {
            task();
        }
        catch (...) // NOLINT(bugprone-empty-catch) : voulu, voir ci-dessous
        {
            // Une exception ne doit pas tuer le fil, sinon l'appli ne répondrait plus.
        }
    }
}

} // namespace bookshelf::bridge
