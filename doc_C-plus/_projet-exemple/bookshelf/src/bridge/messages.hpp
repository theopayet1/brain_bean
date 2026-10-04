#pragma once

#include <string_view>

namespace bookshelf::bridge
{

// La phrase à montrer pour un code d'erreur. Un seul endroit pour tous les textes.
[[nodiscard]] std::string_view messageFor(std::string_view code);

} // namespace bookshelf::bridge
