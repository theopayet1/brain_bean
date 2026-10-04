---
tags:
  - projet/cpp
  - type/guide
  - techno/sqlite
  - sujet/base-de-donnees
  - statut/a-jour
aliases:
  - Transactions SQLite
cree: 2026-10-04
maj: 2026-10-04
---

# Transactions

> [!abstract] En une phrase
> Une **transaction** regroupe plusieurs écritures : **toutes** sont enregistrées (`COMMIT`), ou **aucune** (`ROLLBACK`). En C++, une classe RAII l'**annule automatiquement** si on n'a pas appelé `commit()` — en cas d'exception ou de `return` anticipé. Côté application, un port `ITransactions` permet aux services de grouper des écritures sans connaître SQLite.

---

## 🧱 La classe RAII

Déjà dans `connection.hpp` ([[01 SQLite — enveloppe RAII]]) :

```cpp
class Transaction
{
public:
    explicit Transaction(Connection& connection);   // BEGIN IMMEDIATE
    ~Transaction();                                  // ROLLBACK si pas validée
    void commit();                                   // COMMIT
};
```

```cpp
{
    Transaction transaction{connection};
    books.create(a);
    books.create(b);       // si ça lève : ROLLBACK, a n'est pas gardé non plus
    transaction.commit();
}
```

> [!info] `BEGIN IMMEDIATE`
> Prend tout de suite le verrou d'écriture. Avec un simple `BEGIN`, la transaction pourrait échouer plus tard, au moment de la première écriture.

---

## 🔌 Le port `ITransactions`

Le service ne doit pas connaître `Transaction` (c'est SQLite). On lui donne un port :

```cpp
// application/ports/transactions.hpp
class ITransactions
{
public:
    ITransactions() = default;
    ITransactions(const ITransactions&) = delete;
    ITransactions& operator=(const ITransactions&) = delete;
    virtual ~ITransactions() = default;

    // Exécute `work` dans une transaction : validée si `work` réussit, annulée s'il renvoie
    // une erreur ou lève une exception (qui est alors propagée).
    [[nodiscard]] virtual domain::Result<void>
    run(const std::function<domain::Result<void>()>& work) = 0;
};
```

L'implémentation SQLite :

```cpp
// infrastructure/sqlite/transactions_sqlite.hpp
class TransactionsSqlite : public application::ITransactions
{
public:
    explicit TransactionsSqlite(Connection& connection) : connection_(connection) {}

    domain::Result<void> run(const std::function<domain::Result<void>()>& work) override
    {
        Transaction transaction{connection_};
        auto result = work();       // une exception traverse : le destructeur annule
        if (result)
        {
            transaction.commit();   // succès : on valide
        }
        return result;              // échec prévu : pas de commit, le destructeur annule
    }

private:
    Connection& connection_;
};
```

La fausse, pour les tests, exécute simplement `work()` :

```cpp
class DirectTransactions : public application::ITransactions
{
public:
    domain::Result<void> run(const std::function<domain::Result<void>()>& work) override
    {
        return work();
    }
};
```

Utilisation dans un service : voir l'exemple `LoanService::lend` dans [[04 Les services (cas d'usage)]].

---

## ⚠️ Pièges

| Symptôme | Cause | Solution |
|---|---|---|
| `cannot start a transaction within a transaction` | Deux `Transaction` imbriquées | Une seule transaction par cas d'usage, ouverte par le **service** |
| `database is locked` | Une autre connexion (ou un autre programme) écrit | Une seule connexion, un seul fil ([[08 Le fil de travail]]) ; `busy_timeout` |
| Écritures perdues | `commit()` oublié | Le test « une transaction non validée est annulée » le rappelle |

---

## 🔗 Liens

- [[02 RAII]] — le principe de la classe
- [[05 JSON avec glaze]] — la suite
