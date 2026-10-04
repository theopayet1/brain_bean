#pragma once

#include <saucer/scheme.hpp>

#include <expected>
#include <string_view>

namespace bookshelf::app
{

// Le frontend est servi depuis l'exe par un schéma personnalisé : pas de serveur HTTP
// local, pas de fichier sur le disque.
inline constexpr std::string_view FrontendScheme = "app";
inline constexpr std::string_view FrontendOrigin = "app://bookshelf";
inline constexpr std::string_view HomePage = "app://bookshelf/index.html";

// Répond à une requête app://bookshelf/<chemin> avec le fichier embarqué correspondant.
[[nodiscard]] std::expected<saucer::scheme::response, saucer::scheme::error>
serveFrontend(const saucer::scheme::request& request);

// Vrai si l'adresse appartient au frontend embarqué. Toute autre navigation est refusée.
[[nodiscard]] bool isFrontendUrl(std::string_view url);

} // namespace bookshelf::app
