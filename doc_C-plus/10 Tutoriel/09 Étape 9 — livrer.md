---
tags:
  - projet/cpp
  - type/guide
  - techno/cmake
  - techno/msvc
  - sujet/build
  - sujet/securite
  - statut/a-jour
aliases:
  - Tutoriel étape 9
  - Livrer une app C++
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 9 — livrer

> [!abstract] En une phrase
> On construit la version **release** (optimisée, sans outils de développement), on vérifie la checklist de sécurité, et on obtient **un seul `.exe`** à donner : il ne dépend d'aucune DLL à installer, seulement du WebView2 Runtime déjà présent sur Windows 11.

---

## 🔨 1. Construire la release

```powershell
.\scripts\check.ps1 -Release
```

L'exe est dans `build\release\src\app\Bookshelf.exe`.

| Différence avec debug | Effet |
|---|---|
| `NDEBUG` défini | `set_dev_tools(false)`, `set_context_menu(false)` |
| Optimisations + `INTERPROCEDURAL_OPTIMIZATION` | Plus rapide, plus petit |
| Pas d'AddressSanitizer | Vitesse normale |
| Runtime C++ statique | Aucune DLL Visual C++ à installer |

## 🔍 2. Vérifier les dépendances de l'exe

```powershell
dumpbin /dependents build\release\src\app\Bookshelf.exe
```

On ne doit voir que des DLL **système** de Windows (`KERNEL32.dll`, `USER32.dll`, `ole32.dll`…), aucune `MSVCP140.dll` ni `sqlite3.dll`.

## 🛡️ 3. La checklist

- [ ] Tous les tests verts en debug **et** en release
- [ ] [[09 Sécuriser la webview]] : checklist complète
- [ ] Aucune donnée personnelle dans `errors.log`
- [ ] Lancement sur un PC « propre » (une machine virtuelle Windows 11 sans Visual Studio)
- [ ] Lancement **sans réseau**
- [ ] Le dossier `%LOCALAPPDATA%\Bookshelf\` est créé au premier lancement
- [ ] Une mise à jour (nouvel exe) ouvre une ancienne base et la migre ([[02 Migrations de schéma]])

## 📦 4. Distribuer

| Façon | Quand |
|---|---|
| Donner l'exe | Usage personnel, un poste |
| Un installeur (Inno Setup, WiX) | Raccourci dans le menu Démarrer, désinstallation propre, installation du WebView2 Runtime si absent |
| Signer l'exe (certificat de signature de code) | Éviter l'avertissement SmartScreen « éditeur inconnu » |

> [!tip] Une seule instance
> Pour empêcher deux fenêtres d'ouvrir la même base : un **mutex nommé** Windows (`CreateMutexW` avec un nom propre à l'appli) au début du `main`. Si `GetLastError() == ERROR_ALREADY_EXISTS`, afficher « L'application est déjà ouverte » et quitter.

---

## 🎉 Et ensuite

Pour chaque nouvelle fonctionnalité, suivre [[05 Ajouter une fonctionnalité]] : domaine → port → service → infrastructure → pont → TypeScript → écran, avec un test à chaque couche.

---

## 🔗 Liens

- [[00 Tutoriel — une app de bureau complète]] — le plan
- [[05 Ajouter une fonctionnalité]] — la checklist pour la suite
