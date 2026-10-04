#include "app/embedded_frontend.hpp"

#include <cmrc/cmrc.hpp>

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <utility>

CMRC_DECLARE(frontend); // NOLINT : code de la macro CMakeRC

namespace bookshelf::app
{

namespace
{

// La page ne peut charger QUE ses propres fichiers et n'ouvrir aucune connexion : même une
// balise ajoutée par erreur vers un CDN ou une police en ligne serait bloquée.
constexpr std::string_view ContentSecurityPolicy =
    "default-src 'none'; script-src 'self'; style-src 'self'; img-src 'self' data:; "
    "font-src 'self'; connect-src 'none'; base-uri 'none'; form-action 'none'; "
    "frame-ancestors 'none'";

constexpr auto MimeTypes = std::to_array<std::pair<std::string_view, std::string_view>>({
    {".html", "text/html; charset=utf-8"},
    {".js", "text/javascript; charset=utf-8"},
    {".css", "text/css; charset=utf-8"},
    {".json", "application/json; charset=utf-8"},
    {".svg", "image/svg+xml"},
    {".png", "image/png"},
    {".ico", "image/x-icon"},
    {".woff2", "font/woff2"},
});

std::string_view mimeType(std::string_view path)
{
    for (const auto& [extension, mime] : MimeTypes)
    {
        if (path.ends_with(extension))
        {
            return mime;
        }
    }
    return "application/octet-stream";
}

// « app://bookshelf/assets/index.js?x#y » → « assets/index.js ».
// Chaîne vide si l'adresse n'est pas celle du frontend.
std::string_view pathInFrontend(std::string_view url)
{
    if (!isFrontendUrl(url))
    {
        return {};
    }
    url.remove_prefix(FrontendOrigin.size());
    url = url.substr(0, url.find_first_of("?#"));
    if (url.starts_with('/'))
    {
        url.remove_prefix(1);
    }
    return url.empty() ? std::string_view{"index.html"} : url;
}

} // namespace

bool isFrontendUrl(std::string_view url)
{
    if (!url.starts_with(FrontendOrigin))
    {
        return false;
    }
    // « app://bookshelf.evil.com » commence aussi par l'origine : on exige une fin d'hôte.
    const std::string_view rest = url.substr(FrontendOrigin.size());
    return rest.empty() || rest.front() == '/' || rest.front() == '?' || rest.front() == '#';
}

std::expected<saucer::scheme::response, saucer::scheme::error>
serveFrontend(const saucer::scheme::request& request)
{
    const std::string url = request.url();
    const std::string_view path = pathInFrontend(url);
    if (path.empty())
    {
        return std::unexpected(saucer::scheme::error::denied);
    }

    const auto files = cmrc::frontend::get_filesystem();
    const std::string name{path};
    if (!files.is_file(name))
    {
        return std::unexpected(saucer::scheme::error::not_found);
    }

    // Les octets vivent dans l'exe pendant toute la durée du programme : une simple vue
    // suffit, rien n'est copié.
    const cmrc::file file = files.open(name);
    const std::span<const std::uint8_t> bytes{
        reinterpret_cast<const std::uint8_t*>(file.begin()), // NOLINT(*-reinterpret-cast)
        file.size(),
    };

    return saucer::scheme::response{
        .data = saucer::stash<>::view(bytes),
        .mime = std::string{mimeType(path)},
        .headers =
            {
                {"Content-Security-Policy", std::string{ContentSecurityPolicy}},
                {"X-Content-Type-Options", "nosniff"},
                {"Cache-Control", "no-store"},
            },
    };
}

} // namespace bookshelf::app
