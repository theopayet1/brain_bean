---
tags:
  - projet/cpp
  - type/guide
  - techno/saucer
  - techno/webview2
  - sujet/ui
  - statut/a-jour
aliases:
  - saucer
  - Fenêtre saucer
cree: 2026-10-04
maj: 2026-10-04
---

# saucer — ouvrir une fenêtre

> [!abstract] En une phrase
> Avec saucer, on crée une **application** (`saucer::application::init`), puis une **fenêtre avec navigateur** (`saucer::webview`), on règle sa taille et son titre, on lui donne une adresse à afficher, on l'affiche, et on lance la **boucle d'événements** (`application->run()`), qui tourne jusqu'à la fermeture.

---

## 👋 Le minimum

```cpp
#include <saucer/webview.hpp>

int main()
{
    auto app = saucer::application::init({.id = "hello"});   // 1. l'application

    saucer::webview window{{.application = app}};            // 2. la fenêtre

    window.set_title("Hello");
    window.set_size(900, 600);
    window.set_url("https://example.com");                   // 3. ce qu'elle affiche

    window.show();                                           // 4. l'afficher
    app->run();                                              // 5. la boucle
    return 0;
}
```

| Code | Pourquoi |
|---|---|
| `application::init({.id = ...})` | Crée l'application (une seule par programme) ; renvoie un `shared_ptr` |
| `saucer::webview window{{...}}` | Les **préférences** : l'application, le dossier du profil, les options du moteur |
| `set_url` | Charge une page. Dans la vraie appli : `app://bookshelf/index.html`, voir [[04 Embarquer le frontend dans l'exe]] |
| `app->run()` | **Bloque** et traite les événements (clics, redessins, messages) jusqu'à la fermeture de la fenêtre |

---

## ⚙️ Les préférences de la fenêtre

```cpp
saucer::webview window{{
    .application = application,
    .persistent_cookies = false,                 // pas de cookies gardés sur le disque
    .storage_path = *folder / "webview",         // le profil du navigateur, dans NOTRE dossier
    .browser_flags = app::browserFlags(),        // options de démarrage du moteur
}};
```

| Préférence | Rôle |
|---|---|
| `application` | **Obligatoire** |
| `persistent_cookies` | Garder les cookies entre deux lancements |
| `hardware_acceleration` | Accélération GPU (vrai par défaut) |
| `storage_path` | Où WebView2 écrit son profil. **À fixer** : sinon il écrit dans le dossier courant, interdit en écriture quand l'appli est installée dans `Program Files` |
| `user_agent` | L'identifiant du navigateur |
| `browser_flags` | Options en ligne de commande du moteur Chromium/Edge, voir [[09 Sécuriser la webview]] |

---

## 🪟 Les réglages de la fenêtre

| Méthode | Effet |
|---|---|
| `set_title("...")` | Titre |
| `set_size(l, h)` / `set_min_size(l, h)` / `set_max_size(l, h)` | Tailles |
| `set_resizable(bool)` / `set_maximized(bool)` | Redimensionnable / maximisée |
| `set_dev_tools(bool)` | Autorise F12 (outils de développement) |
| `set_context_menu(bool)` | Clic droit du navigateur |
| `set_background({r, g, b, a})` | Couleur avant que la page soit chargée |
| `set_url(...)` / `set_file(...)` | Charger une adresse / un fichier local |
| `execute("code js")` | Exécuter du JavaScript dans la page |
| `show()` / `hide()` / `close()` | — |

---

## 📣 Les événements

```cpp
// Avant chaque navigation : on autorise ou on bloque.
window.on<saucer::web_event::navigate>(
    [](const saucer::navigation& navigation)
    {
        return app::isFrontendUrl(navigation.url()) ? saucer::policy::allow
                                                    : saucer::policy::block;
    });

// La fenêtre va se fermer : on peut refuser (« modifications non enregistrées »).
window.on<saucer::window_event::close>([] { return saucer::policy::allow; });
```

| Événement | Quand |
|---|---|
| `web_event::navigate` | Avant d'aller à une adresse (renvoie `allow` / `block`) |
| `web_event::navigated` | Après |
| `web_event::dom_ready` | Le HTML est prêt |
| `web_event::load` | Début / fin de chargement |
| `window_event::close` | Avant la fermeture (renvoie `allow` / `block`) |
| `window_event::closed` | Après |
| `window_event::resize` | Taille changée |

---

## 🧩 Les modules et les schémas

- **Module** : une classe qu'on attache à la fenêtre avec `window.add_module<T>(args...)`. saucer appelle sa méthode `on_message` à chaque message envoyé par le JavaScript. C'est notre point d'entrée du pont, voir [[08 Le fil de travail]].
- **Schéma** : une adresse maison (`app://...`) servie par une fonction C++. Voir [[04 Embarquer le frontend dans l'exe]].

```cpp
saucer::webview::register_scheme("app");                    // AVANT application::init
// ...
window.handle_scheme("app", app::serveFrontend);
window.add_module<app::MessageReceiver>(onMessage);
```

---

## 🧵 Les fils d'exécution de saucer

| Fil | Ce qui s'y passe |
|---|---|
| **Fil de l'interface** (celui de `run()`) | Événements, `on_message`, redessins. **Ne jamais le bloquer** |
| Ton fil de travail | Base de données, calculs longs |

`application->post(fonction)` dépose une fonction à exécuter **sur le fil de l'interface** : c'est comme ça qu'on renvoie une réponse à la page depuis le fil de travail. Voir [[08 Le fil de travail]].

> [!warning] WebView2 Runtime
> Sur Windows 11, il est toujours présent. Sur un Windows 10 ancien, il peut manquer : prévoir l'installeur « Evergreen Bootstrapper » de Microsoft avec l'appli, ou un message clair au démarrage.

---

## 🔗 Liens

- [[06 La racine de composition]] — la fenêtre dans le vrai `main`
- [[03 Le frontend (Vite, TypeScript, Preact)]] — la suite
