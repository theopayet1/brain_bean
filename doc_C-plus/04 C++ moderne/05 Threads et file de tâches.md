---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - sujet/concurrence
  - statut/a-jour
aliases:
  - Threads C++
  - std::jthread
  - Mutex C++
cree: 2026-10-04
maj: 2026-10-04
---

# Threads et file de tâches

> [!abstract] En une phrase
> Un **fil d'exécution** (*thread*) exécute du code **en même temps** que les autres. C'est indispensable pour qu'une interface ne se fige pas, mais dangereux : deux fils qui touchent la même donnée sans **verrou** donnent un comportement indéfini. La solution la plus simple et la plus sûre : **un seul fil de travail** avec une **file de tâches**, et rien d'autre ne touche aux données.

---

## 🧵 Lancer un fil : `std::jthread`

```cpp
#include <thread>

{
    std::jthread worker([] { doLongWork(); });   // démarre tout de suite
    // ... le fil principal continue ...
}   // 👈 le destructeur de jthread ATTEND la fin du fil (join)
```

> [!tip] `jthread` et pas `thread`
> Un `std::thread` détruit sans `join()` **arrête le programme**. `std::jthread` (C++20) fait le `join` tout seul : c'est du [[02 RAII]].

---

## 🔒 Protéger une donnée partagée : `mutex`

```cpp
#include <mutex>

std::mutex mutex;
std::vector<int> shared;

void add(int value)
{
    const std::scoped_lock lock{mutex};   // 👈 verrouille ici…
    shared.push_back(value);
}                                          // 👈 …déverrouille ici, même en cas d'exception
```

> [!warning] Course de données (*data race*)
> Deux fils qui modifient (ou l'un modifie, l'autre lit) la même variable **sans verrou** = comportement indéfini. Le bug apparaît une fois sur mille, jamais en debug.

---

## 😴 Attendre un événement : `condition_variable`

Un fil qui n'a rien à faire doit **dormir**, pas tourner en boucle. Une **variable de condition** le réveille quand il y a du travail.

```cpp
std::condition_variable wakeUp;

// Le fil qui attend :
std::unique_lock lock{mutex};
wakeUp.wait(lock, [] { return !tasks.empty(); });   // dort tant que la condition est fausse

// Le fil qui donne du travail :
{
    const std::scoped_lock lock{mutex};
    tasks.push_back(task);
}
wakeUp.notify_one();                                // réveille le dormeur
```

---

## 🏭 La file de tâches complète

Le motif qu'on utilise pour toute l'application : **un** fil qui exécute les tâches **une par une**, dans l'ordre.

`src/bridge/worker.hpp` :

```cpp
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
```

`src/bridge/worker.cpp` :

```cpp
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
        catch (...)
        {
            // Une exception ne doit pas tuer le fil, sinon l'appli ne répondrait plus.
        }
    }
}

} // namespace bookshelf::bridge
```

| Code | Pourquoi |
|---|---|
| `std::move_only_function` | Une tâche peut contenir un objet non copiable (un message déplacé) |
| `std::deque` | Ajout à la fin, retrait au début : rapide aux deux bouts |
| `thread_` déclaré **en dernier** | Les membres sont construits dans l'ordre : le fil ne démarre qu'une fois `mutex_`, `tasks_`… prêts. Et il est détruit **en premier** : il s'arrête avant eux |
| `wait(lock, condition)` | Se protège des « faux réveils » en revérifiant la condition |
| `if (tasks_.empty()) return;` | À l'arrêt, on **vide d'abord** la file : aucune tâche postée n'est perdue |
| Tâche exécutée hors du verrou | Sinon `post()` bloquerait pendant une sauvegarde de 3 secondes |
| `catch (...)` | Une tâche qui lève ne doit pas tuer l'application |

> [!example] Le guichet unique
> Une seule personne au guichet (le fil de travail). Les clients (l'interface) déposent leur demande dans la boîte et repartent. Le guichetier traite les demandes **une par une**. Comme il est seul, il n'a jamais besoin de se disputer un dossier avec un collègue.

Utilisation et tests : [[08 Le fil de travail]].

---

## 🔗 Liens

- [[08 Le fil de travail]] — brancher la file sur l'interface
- [[06 const, constexpr, noexcept et nodiscard]] — la suite
