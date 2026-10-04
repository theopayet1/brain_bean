---
tags:
  - projet/cpp
  - type/concept
  - techno/cpp
  - sujet/memoire
  - statut/a-jour
aliases:
  - RAII
  - Resource Acquisition Is Initialization
cree: 2026-10-04
maj: 2026-10-04
---

# RAII

> [!abstract] En une phrase
> **RAII** (« l'acquisition d'une ressource est une initialisation ») : on confie chaque ressource à un objet dont le **constructeur la prend** et le **destructeur la rend**. Comme le destructeur est appelé **toujours** (fin de bloc, `return`, exception), la ressource ne fuit jamais. C'est **l'idée la plus importante du C++**.

---

## 🔑 Le problème

```cpp
void save()
{
    sqlite3* db = nullptr;
    sqlite3_open("books.db", &db);
    if (somethingWrong())
    {
        return;              // ❌ oubli : la base reste ouverte
    }
    mayThrow();              // ❌ si ça lève une exception : la base reste ouverte
    sqlite3_close(db);
}
```

## ✅ La solution RAII

```cpp
void save()
{
    Connection connection{"books.db"};   // le constructeur ouvre
    if (somethingWrong())
    {
        return;              // ✅ le destructeur ferme
    }
    mayThrow();              // ✅ le destructeur ferme aussi
}                            // ✅ et ici aussi
```

> [!example] Le vestiaire
> Tu donnes ton manteau (la ressource) et on te donne un **ticket** (l'objet RAII). Quand tu pars, **quoi qu'il arrive** — tu pars normalement, par l'issue de secours, en courant — le ticket est rendu et le manteau aussi. Tu ne peux pas oublier.

---

## 🏗️ Écrire une classe RAII : la transaction

Une transaction SQLite doit être **validée** (`COMMIT`) si tout va bien, **annulée** (`ROLLBACK`) sinon.

```cpp
class Transaction
{
public:
    explicit Transaction(Connection& connection);
    ~Transaction();

    Transaction(const Transaction&) = delete;             // 👈 pas de copie
    Transaction& operator=(const Transaction&) = delete;

    void commit();

private:
    Connection& connection_;
    bool done_ = false;
};

Transaction::Transaction(Connection& connection)
    : connection_(connection)
{
    connection_.execute("BEGIN IMMEDIATE;");     // 👈 prendre
}

Transaction::~Transaction()
{
    if (!done_)
    {
        try
        {
            connection_.execute("ROLLBACK;");    // 👈 rendre, si pas validée
        }
        catch (...)                               // un destructeur ne doit JAMAIS lever
        {
        }
    }
}

void Transaction::commit()
{
    connection_.execute("COMMIT;");
    done_ = true;
}
```

Utilisation :

```cpp
{
    Transaction transaction{connection};
    books.create(dune);
    books.create(foundation);   // si ça lève, ROLLBACK : aucun des deux n'est gardé
    transaction.commit();
}
```

| Code | Pourquoi |
|---|---|
| `= delete` sur la copie | Deux objets qui annulent la même transaction : absurde |
| `done_` | Le destructeur sait s'il doit annuler |
| `try / catch (...)` dans le destructeur | Une exception qui sort d'un destructeur **arrête le programme** |

---

## 🧰 Les objets RAII de la bibliothèque standard

| Ressource | Objet RAII |
|---|---|
| Mémoire | `std::vector`, `std::string`, `std::unique_ptr` |
| Fichier | `std::ifstream`, `std::ofstream` |
| Verrou d'un mutex | `std::scoped_lock`, `std::unique_lock` |
| Fil d'exécution | `std::jthread` (attend la fin du fil à la destruction) |
| Ressource d'une API C (`sqlite3*`, `HANDLE`) | `std::unique_ptr` avec **supprimeur personnalisé**, voir [[03 Pointeurs intelligents]] |

---

## 📐 Ordre de destruction

Les objets sont détruits dans **l'ordre inverse** de leur création. On s'en sert pour que les choses s'éteignent proprement :

```cpp
int main()
{
    Connection connection{path};         // 1. créé en premier
    BooksSqlite books{connection};       // 2. utilise connection
    BookService service{books, clock};   // 3. utilise books
    Worker worker;                       // 4. créé en DERNIER
    // ...
}   // détruits : worker (finit ses tâches), service, books, connection (ferme la base)
```

Voir [[06 La racine de composition]].

---

## 🔗 Liens

- [[03 Pointeurs intelligents]] — RAII pour la mémoire et les API en C
- [[04 Transactions]] — la transaction dans la couche données
