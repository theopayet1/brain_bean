#include "app/window.hpp"

// L'ordre compte et ne doit pas être trié : WebView2.h emploie le mot-clé COM « interface »,
// défini par objbase.h, que windows.h n'apporte pas avec WIN32_LEAN_AND_MEAN.
// clang-format off
#include <windows.h>
#include <objbase.h>
#include <saucer/modules/stable/webview2.hpp>
#include <wrl/client.h>
// clang-format on

#include <utility>

namespace bookshelf::app
{

using Microsoft::WRL::ComPtr;

MessageReceiver::MessageReceiver(saucer::webview* /*window*/, OnMessage onMessage)
    : onMessage_(std::move(onMessage))
{
}

bool MessageReceiver::on_message(const std::string& message)
{
    if (!message.starts_with(BridgePrefix))
    {
        return false; // pas pour nous : saucer s'en occupe
    }
    onMessage_(message.substr(BridgePrefix.size()));
    return true;
}

void hardenWebView(saucer::webview& window)
{
    const auto native = window.native();
    if (native.webview == nullptr)
    {
        return;
    }
    ComPtr<ICoreWebView2Settings> settings;
    if (FAILED(native.webview->get_Settings(&settings)))
    {
        return;
    }
    // Par défaut WebView2 retient ce qui est tapé dans les formulaires, en clair, dans son
    // profil. Les données de l'application doivent rester dans SA base.
    if (ComPtr<ICoreWebView2Settings4> autofill; SUCCEEDED(settings.As(&autofill)))
    {
        autofill->put_IsGeneralAutofillEnabled(FALSE);
        autofill->put_IsPasswordAutosaveEnabled(FALSE);
    }
    // SmartScreen interroge un service en ligne ; l'application ne charge que ses fichiers.
    if (ComPtr<ICoreWebView2Settings8> reputation; SUCCEEDED(settings.As(&reputation)))
    {
        reputation->put_IsReputationCheckingRequired(FALSE);
    }
    // Pas d'historique de pages : pas de retour arrière par glissement.
    if (ComPtr<ICoreWebView2Settings6> gestures; SUCCEEDED(settings.As(&gestures)))
    {
        gestures->put_IsSwipeNavigationEnabled(FALSE);
    }
}

std::set<std::string> browserFlags()
{
    // Le moteur Edge fait de lui-même des requêtes de fond (mises à jour, mesures...).
    // L'application reste muette sur le réseau.
    return {
        // Garde-fou : tout trafic HTTP(S) passe par un mandataire qui n'existe pas, donc
        // échoue. Le frontend, servi par app://, n'est pas concerné.
        "--proxy-server=http://127.0.0.1:9",
        "--disable-background-networking",
        "--disable-component-update",
        "--disable-domain-reliability",
        "--disable-sync",
        "--no-pings",
        "--no-first-run",
    };
}

std::string responseScript(std::string_view responseJson)
{
    // La réponse est du JSON, donc une expression JavaScript valide : elle est passée telle
    // quelle, sans chaîne à échapper.
    std::string script = "window.__bookshelf_response(";
    script += responseJson;
    script += ");";
    return script;
}

} // namespace bookshelf::app
