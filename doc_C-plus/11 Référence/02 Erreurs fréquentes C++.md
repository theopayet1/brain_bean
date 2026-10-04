---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - techno/msvc
  - techno/cmake
  - sujet/debug
  - sujet/compilation
  - statut/a-jour
aliases:
  - Erreurs C++
  - Messages d'erreur MSVC
cree: 2026-10-04
maj: 2026-10-04
---

# Erreurs fréquentes C++

> [!abstract] En une phrase
> Les messages du compilateur C++ sont longs, mais presque toujours l'une de ces quelques erreurs. Règle générale : **lire la première erreur**, pas la dernière, et chercher la première ligne qui cite **ton** fichier.

---

## ⚙️ Configuration (CMake, vcpkg)

| Symptôme | Cause | Solution |
|---|---|---|
| `Could not find toolchain file: /scripts/buildsystems/vcpkg.cmake` | `VCPKG_ROOT` vide | Définir la variable, rouvrir le terminal / l'IDE |
| `Could not find a package configuration file provided by "X"` | Bibliothèque absente de `vcpkg.json` | L'ajouter ; vérifier le nom exact du paquet CMake |
| `the manifest has no baseline` | `builtin-baseline` manquant | `vcpkg x-update-baseline --add-initial-baseline` |
| `No CMAKE_CXX_COMPILER could be found` | Terminal sans l'environnement MSVC | Developer PowerShell, ou toolchain Visual Studio dans CLion |
| `Échec de npm ...` | Node absent ou `package-lock.json` désynchronisé | Installer Node, `npm install` dans `frontend/` |

## 🧱 Compilation

| Symptôme | Cause | Solution |
|---|---|---|
| `C1083: Cannot open include file` | Fichier absent, mauvais chemin, ou cible pas liée | Vérifier le chemin depuis `src/` et le `target_link_libraries` |
| `C2065: identificateur non déclaré` | Faute de frappe, include ou `namespace` manquant | Ajouter l'include / `domain::` devant |
| `C2039: 'x' n'est pas membre de 'std'` | Include manquant (`<format>`, `<optional>`…) | Inclure l'en-tête standard |
| `C2664: impossible de convertir l'argument` | Mauvais type passé | Lire le type attendu dans le message |
| `C2280: tentative de référencement d'une fonction supprimée` | Copie d'un objet non copiable (`unique_ptr`, `Transaction`) | `std::move`, ou passer par référence |
| `C2512: aucun constructeur par défaut approprié` | Membre référence ou classe sans constructeur par défaut | Initialiser dans la liste d'initialisation |
| `C4100: paramètre formel non référencé` (avec `/WX`) | Paramètre inutilisé | Commenter son nom : `int /*show*/` |
| `C4244: conversion, perte possible de données` | `int64_t` → `int`, `double` → `int` | `static_cast<int>(...)` si c'est voulu |
| `C4834: valeur de retour ignorée [[nodiscard]]` | Résultat ignoré | Le traiter, ou `(void)` si vraiment voulu |
| Erreur sur des dizaines de lignes dans `<xmemory>` | Erreur de template | Remonter jusqu'à la ligne de **ton** fichier |
| `declared using local type ..., never defined` (glaze) | Struct dans une fonction ou un namespace anonyme | La déclarer dans un namespace nommé |

## 🔗 Édition de liens

| Symptôme | Cause | Solution |
|---|---|---|
| `LNK2019: symbole externe non résolu` | Fonction déclarée, jamais définie ; `.cpp` absent de CMake ; bibliothèque pas liée | Voir [[04 De la source à l'exe]] |
| `LNK2005: déjà défini` | Définition dans un `.hpp` inclus plusieurs fois | `inline`, ou déplacer dans le `.cpp` |
| `LNK2038: mismatch detected for 'RuntimeLibrary'` | Runtime statique / dynamique mélangé | `CMAKE_MSVC_RUNTIME_LIBRARY` cohérent avec le triplet |
| `LNK1104: impossible d'ouvrir le fichier 'Bookshelf.exe'` | L'appli tourne encore | La fermer |

## 💥 Exécution

| Symptôme | Cause | Solution |
|---|---|---|
| Plantage « access violation » | `nullptr`, objet détruit | Debug + AddressSanitizer ([[07 Déboguer et sanitizers]]) |
| `heap-use-after-free` (ASan) | Référence / vue vers un objet détruit | [[05 Durée de vie et pièges]] |
| L'appli se ferme sans rien dire | Exception non attrapée, `std::terminate` | Lancer en debug, regarder la pile d'appels |
| La fenêtre gèle | Travail long sur le fil de l'interface | [[08 Le fil de travail]] |
| Un clic ne fait rien | Promesse JS sans gestion d'erreur, ou fonction absente de la table du pont | Console F12 ; `errors.log` |
| `database is locked` | Deux connexions, ou une transaction restée ouverte | Une seule connexion sur un seul fil |
| `bridge.invalid_request` | Le JSON envoyé ne correspond pas au DTO (clé en trop / manquante) | Comparer `types.ts` et `dto.hpp` |
| `bridge.unknown_function` | Nom de fonction différent entre `api.ts` et `Bridge::find` | Les aligner |

---

## 🔗 Liens

- [[07 Déboguer et sanitizers]] — les outils
- [[03 Glossaire C++]] — les mots des messages
