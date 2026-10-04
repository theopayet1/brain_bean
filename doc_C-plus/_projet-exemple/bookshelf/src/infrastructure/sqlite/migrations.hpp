#pragma once

#include "infrastructure/sqlite/connection.hpp"

namespace bookshelf::infrastructure::sqlite
{

// Amène le schéma de la base à la dernière version. Sans effet si elle y est déjà.
void migrate(Connection& connection);

} // namespace bookshelf::infrastructure::sqlite
