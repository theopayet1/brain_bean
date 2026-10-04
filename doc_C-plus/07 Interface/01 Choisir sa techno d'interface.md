---
tags:
  - projet/cpp
  - type/decision
  - techno/cpp
  - techno/saucer
  - sujet/ui
  - statut/a-jour
aliases:
  - Qt ou webview
  - GUI C++
cree: 2026-10-04
maj: 2026-10-04
---

# Choisir sa techno d'interface

> [!abstract] En une phrase
> Le C++ n'a pas d'interface graphique « officielle ». Les quatre grandes options sont **Qt**, **Dear ImGui**, **wxWidgets** et une **webview** (saucer, webview/webview). Pour une application de gestion moderne et jolie, faite par une petite équipe, la **webview** est le meilleur compromis : toute la puissance du HTML/CSS pour l'affichage, le C++ pour le reste.

---

## ⚖️ Comparaison

| | **Webview (saucer)** | **Qt** (Widgets / QML) | **Dear ImGui** | **wxWidgets** |
|---|---|---|---|---|
| Apparence | Ce que tu veux (CSS) | Native ou personnalisée (QML) | « Outil de dev » | Native, datée |
| Apprendre | HTML/CSS/TS : immense documentation | Gros framework à apprendre | Très simple | Moyen |
| Formulaires, tableaux, mise en page | Excellent (le web est fait pour ça) | Très bon | Pénible | Correct |
| Taille de l'exe | ~1 à 5 Mo (le moteur est dans Windows) | 20 à 50 Mo de DLL | Petit | Moyen |
| Licence | MIT | LGPL / commerciale | MIT | Libre |
| Accès C++ ⇄ UI | Par messages (pont) | Direct (signaux / slots) | Direct | Direct |
| Idéal pour | Applications de gestion, outils métier | Grosses applis multi-plateformes | Outils internes, jeux, debug | Applis classiques |

---

## ✅ Le choix de cette doc : saucer + WebView2 + Preact

| Sujet | Décision | Pourquoi |
|---|---|---|
| Moteur | **WebView2** (Edge) | Présent sur Windows 11, à jour automatiquement, rapide |
| Bibliothèque C++ | **saucer 6** | C++ moderne, schémas personnalisés, modules, multi-plateforme |
| Pont | **Protocole maison** par-dessus `saucer::webview` | Le `smartview` de saucer (fonctions exposées automatiquement) existe, mais un protocole maison garde le pont **indépendant de saucer** et **testable sans fenêtre** |
| Framework JS | **Preact** | L'API de React en 4 Ko : composants, `useState`, `useEffect` |
| Langage | **TypeScript** strict | Les erreurs de types attrapées avant de lancer |
| Build | **Vite** | Rapide, simple, serveur de dev avec rechargement à chaud |
| Embarquement | **CMakeRC** | Un seul exe, rien sur le disque |

> [!info] `smartview` : l'autre façon de faire avec saucer
> saucer propose `saucer::smartview`, qui expose directement des fonctions C++ au JavaScript :
> ```cpp
> saucer::smartview webview{{.application = app}};
> webview.expose("add", [](int a, int b) { return a + b; });
> // JS : await saucer.exposed.add(1, 2)
> ```
> C'est plus court, mais le pont dépend alors de saucer et de la version de glaze qu'il embarque, et il faut lancer une fenêtre pour le tester. Le protocole maison décrit dans [[05 Le pont C++ JavaScript — le protocole]] évite ces deux problèmes.

---

## 🧠 Ce que ça implique

- **Toute la logique reste en C++.** Le JavaScript ne calcule rien d'important, ne valide rien de définitif, ne touche à aucune donnée : il **affiche** et **demande**.
- Deux langages, deux outillages (CMake + npm), reliés par CMake ([[04 Embarquer le frontend dans l'exe]]).
- L'interface se développe **seule** dans un navigateur, avec une simulation du C++ ([[12 Travailler l'interface sans le C++]]).

---

## 🔗 Liens

- [[02 saucer — ouvrir une fenêtre]] — la suite
- [[05 Le pont C++ JavaScript — le protocole]] — le pont maison
