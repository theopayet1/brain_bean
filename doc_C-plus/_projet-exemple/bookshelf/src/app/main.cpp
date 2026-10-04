#include "app/embedded_frontend.hpp"
#include "app/identity.hpp"
#include "app/window.hpp"
#include "application/books/book_service.hpp"
#include "bridge/bridge.hpp"
#include "bridge/worker.hpp"
#include "infrastructure/sqlite/books_sqlite.hpp"
#include "infrastructure/sqlite/connection.hpp"
#include "infrastructure/sqlite/migrations.hpp"
#include "infrastructure/system/file_log.hpp"
#include "infrastructure/system/system_clock.hpp"
#include "infrastructure/windows/data_folder.hpp"
#include "infrastructure/windows/text.hpp"

#include <saucer/webview.hpp>

#include <windows.h>

#include <exception>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace
{

void showError(std::string_view message)
{
    using bookshelf::infrastructure::toUtf16;
    MessageBoxW(nullptr,
                toUtf16(message).c_str(),
                toUtf16(bookshelf::app::AppName).c_str(),
                MB_OK | MB_ICONERROR);
}

} // namespace

// RACINE DE COMPOSITION : le seul endroit qui connaît toutes les couches et les assemble.
// Pas de conteneur d'injection : les objets sont construits à la main, dans l'ordre de leurs
// dépendances, et détruits dans l'ordre inverse.
int WINAPI wWinMain(HINSTANCE /*instance*/,
                    HINSTANCE /*previous*/,
                    PWSTR /*commandLine*/,
                    int /*show*/)
{
    using namespace bookshelf;

    // 1. Où ranger les données.
    const auto folder = infrastructure::dataFolder(app::AppName);
    if (!folder)
    {
        showError("Impossible d'accéder au dossier des données de l'application.");
        return 1;
    }

    // 2. Les services techniques.
    infrastructure::FileLog log{*folder / "errors.log"};
    const infrastructure::SystemClock clock;

    // 3. La base : ouverte et mise à jour avant tout le reste.
    std::optional<infrastructure::sqlite::Connection> connection;
    try
    {
        connection.emplace(*folder / "bookshelf.db");
        infrastructure::sqlite::migrate(*connection);
    }
    catch (const std::exception& error)
    {
        log.error("démarrage", error.what());
        showError("Impossible d'ouvrir la base de données.");
        return 1;
    }

    // 4. Dépôts, services, pont : chacun reçoit ce dont il a besoin par son constructeur.
    infrastructure::sqlite::BooksSqlite books{*connection};
    application::BookService bookService{books, clock};
    bridge::Bridge bridge{bookService,
                          [&log](std::string_view function, std::string_view detail)
                          {
                              log.error(function, detail);
                          }};

    // 5. La fenêtre. Les schémas personnalisés se déclarent AVANT de créer l'application.
    saucer::webview::register_scheme(std::string{app::FrontendScheme});
    auto application = saucer::application::init({.id = "bookshelf"});

    saucer::webview window{{
        .application = application,
        .persistent_cookies = false,
        // Sans chemin explicite, WebView2 écrit son profil dans le dossier courant :
        // refusé quand l'exe est installé dans Program Files.
        .storage_path = *folder / "webview",
        .browser_flags = app::browserFlags(),
    }};

    window.set_title(std::string{app::AppName});
    window.set_size(1100, 720);
    window.set_min_size(800, 500);
#ifdef NDEBUG
    window.set_dev_tools(false);
    window.set_context_menu(false);
#else
    window.set_dev_tools(true); // F12 en debug
#endif

    app::hardenWebView(window);
    window.handle_scheme(std::string{app::FrontendScheme}, app::serveFrontend);

    // L'application ne quitte jamais son propre frontend : ni lien externe, ni autre site.
    window.on<saucer::web_event::navigate>(
        [](const saucer::navigation& navigation) {
            return app::isFrontendUrl(navigation.url()) ? saucer::policy::allow
                                                        : saucer::policy::block;
        });

    // 6. Le fil de travail. Déclaré en DERNIER, donc détruit en PREMIER : il finit ses tâches
    // avant que le pont, les services et la base ne disparaissent.
    bridge::Worker worker;

    window.add_module<app::MessageReceiver>(
        [&](std::string message)
        {
            // Reçu sur le fil de l'interface, traité sur le fil de travail : la fenêtre ne
            // gèle jamais, même pendant une opération longue.
            worker.post(
                [&, message = std::move(message)]
                {
                    std::string script = app::responseScript(bridge.handle(message));
                    // post() ne bloque pas : la réponse repart vers le fil de l'interface.
                    application->post([&window, script = std::move(script)]
                                      { window.execute(script); });
                });
        });

    window.set_url(std::string{app::HomePage});
    window.show();

    application->run(); // la boucle d'événements : rend la main quand la fenêtre est fermée
    return 0;
}
