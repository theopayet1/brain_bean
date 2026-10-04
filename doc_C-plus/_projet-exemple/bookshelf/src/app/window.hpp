#pragma once

#include <saucer/webview.hpp>

#include <functional>
#include <set>
#include <string>
#include <string_view>

namespace bookshelf::app
{

// Les messages du JavaScript destinés au pont commencent par ce préfixe ; tout autre message
// appartient à saucer lui-même.
inline constexpr std::string_view BridgePrefix = "bookshelf:";

// Module saucer : reçoit les messages postés par le JavaScript (window.chrome.webview
// .postMessage) et passe au pont ceux qui lui sont destinés. Appelé sur le fil de l'interface.
class MessageReceiver
{
public:
    using OnMessage = std::function<void(std::string)>;

    MessageReceiver(saucer::webview* window, OnMessage onMessage);

    // Nom imposé par saucer. Vrai si le message a été pris en charge.
    bool on_message(const std::string& message); // NOLINT(readability-identifier-naming)

private:
    OnMessage onMessage_;
};

// Réglages WebView2 : pas d'autoremplissage des formulaires, pas de SmartScreen, pas de
// navigation par glissement.
void hardenWebView(saucer::webview& window);

// Options de démarrage du moteur : aucune activité réseau de fond.
[[nodiscard]] std::set<std::string> browserFlags();

// Le code JavaScript qui rend une réponse du pont à l'interface.
[[nodiscard]] std::string responseScript(std::string_view responseJson);

} // namespace bookshelf::app
