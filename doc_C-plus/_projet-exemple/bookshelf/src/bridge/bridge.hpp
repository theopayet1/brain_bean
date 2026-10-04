#pragma once

#include "application/books/book_service.hpp"
#include "domain/common/result.hpp"

#include <functional>
#include <string>
#include <string_view>

// Le pont : l'équivalent des contrôleurs d'une API web. Il traduit le JSON reçu en appel de
// service, et le résultat en JSON. AUCUNE règle métier ici.
//
// Il ne connaît pas la fenêtre : il reçoit un texte et en renvoie un. On peut donc le tester
// sans interface.
//
//   demande : { "id": 7, "function": "listBooks", "request": { "search": "" } }
//   réponse : { "id": 7, "ok": true, "data": [...] }
//          ou { "id": 7, "ok": false, "code": "book.not_found", "message": "..." }
namespace bookshelf::bridge
{

class Bridge
{
public:
    // Où écrire les erreurs techniques (fichier journal dans l'appli, rien dans les tests).
    using Log = std::function<void(std::string_view function, std::string_view detail)>;

    Bridge(application::BookService& books, Log log);

    // Ne lève JAMAIS : toute erreur devient une réponse `ok: false`.
    [[nodiscard]] std::string handle(std::string_view message) noexcept;

private:
    // Le JSON de `data`, ou une erreur.
    using Response = domain::Result<std::string>;
    using Handler = Response (Bridge::*)(std::string_view request);

    struct Entry
    {
        std::string_view name;
        Handler handler;
    };

    [[nodiscard]] static const Entry* find(std::string_view name);

    Response listBooks(std::string_view request);
    Response addBook(std::string_view request);
    Response markAsRead(std::string_view request);
    Response removeBook(std::string_view request);

    application::BookService& books_;
    Log log_;
};

} // namespace bookshelf::bridge
