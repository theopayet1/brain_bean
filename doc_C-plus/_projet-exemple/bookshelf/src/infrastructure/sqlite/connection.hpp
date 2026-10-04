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
