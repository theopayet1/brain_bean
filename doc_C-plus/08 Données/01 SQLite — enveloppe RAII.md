---
tags:
  - projet/cpp
  - type/guide
  - techno/sqlite
  - techno/cpp
  - sujet/base-de-donnees
  - sujet/memoire
  - statut/a-jour
aliases:
  - Wrapper SQLite C++
  - Connection Statement
cree: 2026-10-04
maj: 2026-10-04
---

# SQLite — enveloppe RAII

> [!abstract] En une phrase
> SQLite s'utilise par une **API C** (`sqlite3_open`, `sqlite3_prepare`, `sqlite3_step`…) où il faut tout libérer à la main et tester chaque code de retour. Trois petites classes la rendent sûre : **`Connection`** (la base ouverte), **`Statement`** (une requête préparée, paramètres et lecture des colonnes), et **`SqliteError`** (une exception pour l'imprévu). Environ 250 lignes, au lieu d'une bibliothèque de plus.

---

## 📄 L'en-tête : `connection.hpp`

```cpp
#pragma once

#include <cstdint>
#include <filesystem>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

struct sqlite3;
struct sqlite3_stmt;

namespace bookshelf::infrastructure::sqlite
{

// Erreur SQLite imprévue (requête fausse, disque plein…). C'est une exception : elle
// remonte jusqu'au pont, qui la journalise et affiche un message générique.
class SqliteError : public std::runtime_error
{
public:
    SqliteError(int code, const std::string& message);

    [[nodiscard]] int code() const noexcept { return code_; }

private:
    int code_;
};

class Statement;

// Une connexion ouverte sur un fichier SQLite. Fermée toute seule à la destruction (RAII).
class Connection
{
public:
    explicit Connection(const std::filesystem::path& file);

    // Exécute une ou plusieurs instructions sans paramètre ni résultat.
    void execute(std::string_view sql);

    [[nodiscard]] Statement prepare(std::string_view sql);

    [[nodiscard]] std::int64_t lastInsertId() const noexcept;
    [[nodiscard]] int changes() const noexcept;

private:
    struct Close
    {
        void operator()(sqlite3* db) const noexcept;
    };

    std::unique_ptr<sqlite3, Close> db_;
};

// Requête préparée. Les paramètres se lient dans l'ordre des « ? » :
//   connection.prepare("INSERT INTO book (title, author) VALUES (?, ?)")
//       .bind(title).bind(author).run();
class Statement
{
public:
    Statement(sqlite3* db, std::string_view sql);

    Statement& bind(std::int64_t value);
    Statement& bind(int value);
    Statement& bind(bool value);
    Statement& bind(std::string_view value);
    // Sans cette surcharge, un littéral "texte" serait converti en bool !
    Statement& bind(const char* value);
    Statement& bind(std::nullopt_t);

    template <typename T>
    Statement& bind(const std::optional<T>& value)
    {
        return value ? bind(*value) : bind(std::nullopt);
    }

    // Avance d'une ligne. Faux quand il n'y en a plus.
    bool next();

    // Pour une instruction sans résultat (INSERT, UPDATE, DELETE).
    void run();

    // Les colonnes se lisent par position, à partir de 0.
    [[nodiscard]] bool isNull(int column) const;
    [[nodiscard]] std::int64_t integer(int column) const;
    [[nodiscard]] std::string text(int column) const;
    [[nodiscard]] std::optional<std::int64_t> integerOrNull(int column) const;

private:
    struct Finalize
    {
        void operator()(sqlite3_stmt* statement) const noexcept;
    };

    void check(int code) const;

    sqlite3* db_;
    std::unique_ptr<sqlite3_stmt, Finalize> statement_;
    int nextParameter_ = 1;
};

// Transaction annulée toute seule si `commit()` n'a pas été appelé :
// une exception en cours de route ne laisse jamais la base à moitié écrite.
class Transaction
{
public:
    explicit Transaction(Connection& connection);
    ~Transaction();

    Transaction(const Transaction&) = delete;
    Transaction& operator=(const Transaction&) = delete;

    void commit();

private:
    Connection& connection_;
    bool done_ = false;
};

} // namespace bookshelf::infrastructure::sqlite
```

## 📄 L'implémentation : `connection.cpp`

```cpp
#include "infrastructure/sqlite/connection.hpp"

#include <sqlite3.h>

#include <format>

namespace bookshelf::infrastructure::sqlite
{

SqliteError::SqliteError(int code, const std::string& message)
    : std::runtime_error(message)
    , code_(code)
{
}

// --- Connection --------------------------------------------------------------------------

void Connection::Close::operator()(sqlite3* db) const noexcept
{
    sqlite3_close_v2(db);
}

Connection::Connection(const std::filesystem::path& file)
{
    sqlite3* raw = nullptr;
    // SQLite veut un chemin UTF-8 en char*. u8string() donne des char8_t : même octets,
    // autre type, d'où le cast.
    const std::u8string path = file.u8string();
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    const int code = sqlite3_open_v2(reinterpret_cast<const char*>(path.c_str()),
                                     &raw,
                                     SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                     nullptr);
    // Même en cas d'échec, SQLite peut allouer la connexion : on la confie au unique_ptr
    // AVANT de lever, pour qu'elle soit fermée.
    db_.reset(raw);
    if (code != SQLITE_OK)
    {
        throw SqliteError(code, std::format("ouverture impossible : {}", sqlite3_errstr(code)));
    }
    // Sans ça, SQLite ignore les REFERENCES ... ON DELETE CASCADE.
    execute("PRAGMA foreign_keys = ON;");
}

void Connection::execute(std::string_view sql)
{
    const std::string copy{sql}; // sqlite3_exec veut une chaîne terminée par '\0'
    char* message = nullptr;
    const int code = sqlite3_exec(db_.get(), copy.c_str(), nullptr, nullptr, &message);
    if (code != SQLITE_OK)
    {
        const std::string text = message != nullptr ? message : sqlite3_errstr(code);
        sqlite3_free(message);
        throw SqliteError(code, text);
    }
}

Statement Connection::prepare(std::string_view sql)
{
    return Statement{db_.get(), sql};
}

std::int64_t Connection::lastInsertId() const noexcept
{
    return sqlite3_last_insert_rowid(db_.get());
}

int Connection::changes() const noexcept
{
    return sqlite3_changes(db_.get());
}

// --- Statement ---------------------------------------------------------------------------

void Statement::Finalize::operator()(sqlite3_stmt* statement) const noexcept
{
    sqlite3_finalize(statement);
}

Statement::Statement(sqlite3* db, std::string_view sql)
    : db_(db)
{
    sqlite3_stmt* raw = nullptr;
    check(sqlite3_prepare_v2(db_, sql.data(), static_cast<int>(sql.size()), &raw, nullptr));
    statement_.reset(raw);
}

void Statement::check(int code) const
{
    if (code != SQLITE_OK && code != SQLITE_ROW && code != SQLITE_DONE)
    {
        throw SqliteError(code, sqlite3_errmsg(db_));
    }
}

Statement& Statement::bind(std::int64_t value)
{
    check(sqlite3_bind_int64(statement_.get(), nextParameter_++, value));
    return *this;
}

Statement& Statement::bind(int value)
{
    return bind(static_cast<std::int64_t>(value));
}

Statement& Statement::bind(bool value)
{
    return bind(static_cast<std::int64_t>(value ? 1 : 0));
}

Statement& Statement::bind(std::string_view value)
{
    // SQLITE_TRANSIENT : SQLite fait sa propre copie, `value` peut disparaître ensuite.
    check(sqlite3_bind_text(statement_.get(),
                            nextParameter_++,
                            value.data(),
                            static_cast<int>(value.size()),
                            SQLITE_TRANSIENT));
    return *this;
}

Statement& Statement::bind(const char* value)
{
    return bind(std::string_view{value});
}

Statement& Statement::bind(std::nullopt_t)
{
    check(sqlite3_bind_null(statement_.get(), nextParameter_++));
    return *this;
}

bool Statement::next()
{
    const int code = sqlite3_step(statement_.get());
    check(code);
    return code == SQLITE_ROW;
}

void Statement::run()
{
    while (next())
    {
    }
}

bool Statement::isNull(int column) const
{
    return sqlite3_column_type(statement_.get(), column) == SQLITE_NULL;
}

std::int64_t Statement::integer(int column) const
{
    return sqlite3_column_int64(statement_.get(), column);
}

std::string Statement::text(int column) const
{
    // SQLite renvoie des « unsigned char » : mêmes octets que des char, d'où le cast.
    const auto* bytes = sqlite3_column_text(statement_.get(), column);
    const int size = sqlite3_column_bytes(statement_.get(), column);
    if (bytes == nullptr)
    {
        return {};
    }
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return std::string{reinterpret_cast<const char*>(bytes), static_cast<std::size_t>(size)};
}

std::optional<std::int64_t> Statement::integerOrNull(int column) const
{
    if (isNull(column))
    {
        return std::nullopt;
    }
    return integer(column);
}

// --- Transaction -------------------------------------------------------------------------

Transaction::Transaction(Connection& connection)
    : connection_(connection)
{
    connection_.execute("BEGIN IMMEDIATE;");
}

Transaction::~Transaction()
{
    if (!done_)
    {
        try
        {
            connection_.execute("ROLLBACK;");
        }
        // NOLINTNEXTLINE(bugprone-empty-catch) : un destructeur ne doit jamais lever
        catch (...)
        {
        }
    }
}

void Transaction::commit()
{
    connection_.execute("COMMIT;");
    done_ = true;
}

} // namespace bookshelf::infrastructure::sqlite
```

---

## 🔍 Les points importants

| Point | Pourquoi |
|---|---|
| `struct sqlite3;` dans le `.hpp` | Déclaration anticipée : les autres fichiers n'incluent pas `<sqlite3.h>` |
| `std::unique_ptr<sqlite3, Close>` | La base est fermée **automatiquement** ([[03 Pointeurs intelligents]]) |
| `db_.reset(raw)` **avant** de lever | SQLite peut allouer la connexion même en cas d'échec : elle doit être fermée |
| `PRAGMA foreign_keys = ON` | Sans lui, SQLite **ignore** les `REFERENCES ... ON DELETE CASCADE` |
| `bind()` renvoie `Statement&` | On enchaîne : `.bind(a).bind(b).run()` |
| `bind(const char*)` | Sans cette surcharge, `bind("texte")` choisirait… `bind(bool)` ! |
| `SQLITE_TRANSIENT` | SQLite copie le texte : la `string_view` peut disparaître après |
| Lecture **par position** (`row.integer(0)`) | Simple et rapide ; le `SELECT` liste toujours les mêmes colonnes dans le même ordre |
| `SqliteError` est une **exception** | Une erreur SQLite est **imprévue** (requête fausse, disque plein) : elle remonte jusqu'au pont |

---

## ✍️ Utilisation

```cpp
Connection connection{"books.db"};

connection.execute("CREATE TABLE IF NOT EXISTS t (x INTEGER);");

connection.prepare("INSERT INTO book (title, year) VALUES (?, ?)")
    .bind("Dune")
    .bind(std::optional<int>{1965})   // optional vide → NULL
    .run();
std::int64_t id = connection.lastInsertId();

auto statement = connection.prepare("SELECT title, year FROM book WHERE id = ?");
statement.bind(id);
if (statement.next())
{
    std::string title = statement.text(0);
    std::optional<std::int64_t> year = statement.integerOrNull(1);
}
```

> [!warning] Jamais de valeur collée dans le SQL
> ```cpp
> connection.execute("SELECT * FROM book WHERE title = '" + title + "'");   // ❌ injection SQL
> connection.prepare("SELECT ... WHERE title = ?").bind(title);             // ✅ paramètre
> ```
> Un titre contenant `'` casserait la requête, et un texte malveillant pourrait la détourner. **Toujours des `?`.**

---

## ⚙️ Réglages utiles à l'ouverture

| PRAGMA | Effet |
|---|---|
| `foreign_keys = ON` | Respect des clés étrangères |
| `journal_mode = WAL` | Lectures et écriture en parallèle, plus robuste aux coupures |
| `synchronous = NORMAL` | Bon compromis vitesse / sécurité avec WAL |
| `busy_timeout` (`sqlite3_busy_timeout(db, 2000)`) | Attendre 2 s au lieu d'échouer si la base est occupée |

---

## 🔗 Liens

- [[02 RAII]] — le principe
- [[02 Migrations de schéma]] — la suite
