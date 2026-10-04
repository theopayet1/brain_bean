#pragma once

#include <filesystem>
#include <optional>
#include <string_view>

namespace bookshelf::infrastructure
{

// %LOCALAPPDATA%\<appName>, créé s'il n'existe pas. C'est là que vivent la base, le journal et
// le profil de la webview. std::nullopt si Windows refuse.
[[nodiscard]] std::optional<std::filesystem::path> dataFolder(std::string_view appName);

} // namespace bookshelf::infrastructure
