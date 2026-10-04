#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// La forme des messages échangés avec l'interface. Le nom de chaque membre EST la clé JSON :
// le renommer change le contrat avec le frontend (frontend/src/bridge/types.ts).
namespace bookshelf::bridge::dto
{

struct Empty
{
};

struct Failure
{
    std::uint64_t id = 0;
    bool ok = false;
    std::string code;
    std::string message; // phrase lisible, jamais de détail technique
};

struct Book
{
    std::int64_t id = 0;
    std::string title;
    std::string author;
    std::optional<int> year;
    bool read = false;
    std::string addedOn; // « AAAA-MM-JJ »
};

struct ListBooksRequest
{
    std::string search;
};

struct AddBookRequest
{
    std::string title;
    std::string author;
    std::optional<int> year;
};

struct MarkAsReadRequest
{
    std::int64_t id = 0;
    bool read = false;
};

struct IdRequest
{
    std::int64_t id = 0;
};

struct Created
{
    std::int64_t id = 0;
};

} // namespace bookshelf::bridge::dto
