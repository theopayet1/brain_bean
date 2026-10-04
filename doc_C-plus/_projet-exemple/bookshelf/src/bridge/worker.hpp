#pragma once

#include <condition_variable>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

namespace bookshelf::bridge
{

// Le fil de travail : UN seul fil qui exécute les tâches l'une après l'autre.
// Le fil de l'interface y dépose les demandes et reste libre : la fenêtre ne gèle jamais.
// Comme une seule tâche tourne à la fois, la base et les services n'ont besoin d'aucun verrou.
class Worker
{
public:
    using Task = std::move_only_function<void()>;

    Worker();

    // Termine les tâches déjà déposées, puis arrête le fil.
    ~Worker();

    Worker(const Worker&) = delete;
    Worker& operator=(const Worker&) = delete;

    void post(Task task);

private:
    void loop();

    std::mutex mutex_;
    std::condition_variable wakeUp_;
    std::deque<Task> tasks_;
    bool stopping_ = false;
    // Déclaré EN DERNIER : le fil démarre quand tout le reste est déjà construit.
    std::jthread thread_;
};

} // namespace bookshelf::bridge
