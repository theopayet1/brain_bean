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
