---
tags:
  - projet/cpp
  - type/setup
  - techno/cpp
  - techno/msvc
  - techno/cmake
  - techno/vcpkg
  - techno/clion
  - sujet/build
  - statut/a-jour
aliases:
  - Installer le C++ sous Windows
cree: 2026-10-04
maj: 2026-10-04
---

# Installer les outils C++

> [!abstract] En une phrase
> Sous Windows, il faut **le compilateur de Microsoft** (via Visual Studio ou ses Build Tools), **CMake** et **Ninja** pour construire, **vcpkg** pour les bibliothèques, **Node.js** pour l'interface web, et un **éditeur** (CLion ou VS Code). Compte une heure la première fois.

---

## 🧰 La liste

| Outil | Rôle | Où le trouver |
|---|---|---|
| **Visual Studio 2022** (Community) ou **Build Tools for Visual Studio 2022** | Le compilateur `cl.exe` (MSVC), l'éditeur de liens, les en-têtes Windows | visualstudio.microsoft.com |
| Charge de travail **« Développement Desktop en C++ »** | À cocher dans *Visual Studio Installer* : elle apporte aussi **CMake**, **Ninja**, **clang-format**, **clang-tidy** | — |
| Composant **« C++ AddressSanitizer »** | Détecte les erreurs mémoire en debug, voir [[07 Déboguer et sanitizers]] | *Visual Studio Installer › Composants individuels* |
| **vcpkg** | Installe les bibliothèques C++ (SQLite, glaze, saucer…) | Composant « vcpkg » de Visual Studio, ou `git clone https://github.com/microsoft/vcpkg` |
| **Node.js 22+** | Construit l'interface web (Vite) | nodejs.org |
| **WebView2 Runtime** | Le moteur web de la fenêtre | Déjà présent sur Windows 11 |
| **CLion** ou **VS Code** | L'éditeur | jetbrains.com / code.visualstudio.com |

---

## 🪜 Pas à pas

### 1. Le compilateur

1. Télécharger **Visual Studio Installer**.
2. Cocher **Développement Desktop en C++**.
3. Dans **Composants individuels**, cocher aussi **C++ AddressSanitizer** et **vcpkg package manager**.
4. Installer.

### 2. vcpkg

Si tu ne l'as pas pris avec Visual Studio :

```powershell
git clone https://github.com/microsoft/vcpkg C:\dev\vcpkg
C:\dev\vcpkg\bootstrap-vcpkg.bat
```

Puis définir la **variable d'environnement** `VCPKG_ROOT` :

```powershell
setx VCPKG_ROOT C:\dev\vcpkg
```

> [!warning] Rouvrir le terminal
> `setx` ne change pas le terminal déjà ouvert. Ferme-le et rouvre-le (et redémarre CLion) pour que `VCPKG_ROOT` soit vu.

### 3. Node.js

Installer la version LTS. Vérifier :

```powershell
node --version
npm --version
```

---

## 🖥️ Le « Developer PowerShell »

Le compilateur MSVC n'est **pas** dans le `PATH` d'un terminal ordinaire. Deux solutions :

| Solution | Comment |
|---|---|
| Ouvrir **Developer PowerShell for VS 2022** | Menu Démarrer : `cl`, `cmake`, `ninja` sont disponibles |
| Laisser **CLion** le faire | Il charge l'environnement MSVC tout seul (voir plus bas) |

Vérifier dans le Developer PowerShell :

```powershell
cl          # affiche « Microsoft (R) C/C++ Optimizing Compiler Version 19.4x »
cmake --version
ninja --version
```

---

## 🧠 Configurer CLion

1. **Settings › Build, Execution, Deployment › Toolchains** : ajouter une toolchain **Visual Studio**, architecture **amd64**.
2. **Settings › … › CMake** : CLion lit `CMakePresets.json` ([[03 CMakePresets]]). Activer les profils `debug` et `release` et leur associer la toolchain *Visual Studio*.
3. Si `VCPKG_ROOT` n'est pas vu : le mettre dans le champ **Environment** du profil CMake.

> [!tip] VS Code
> Installer les extensions **C/C++** et **CMake Tools** de Microsoft. Lancer VS Code **depuis le Developer PowerShell** (`code .`) pour qu'il trouve le compilateur.

---

## 🐧 Et sous Linux / macOS ?

Le C++ des sections 2 à 4 marche partout. Remplace MSVC par **GCC 14+** ou **Clang 18+** :

```bash
sudo apt install g++ cmake ninja-build    # Ubuntu
```

L'interface (saucer) existe aussi sous Linux (WebKitGTK) et macOS (WKWebView), mais cette doc détaille la version **Windows / WebView2**.

---

## ⚠️ Problèmes fréquents

| Symptôme | Cause | Solution |
|---|---|---|
| `'cl' n'est pas reconnu` | Terminal ordinaire | Utiliser le **Developer PowerShell** |
| `Could not find toolchain file: /scripts/buildsystems/vcpkg.cmake` | `VCPKG_ROOT` vide | Définir `VCPKG_ROOT`, rouvrir le terminal / CLion |
| La première configuration dure 20 minutes | vcpkg compile chaque bibliothèque | Normal, **une seule fois** : le résultat est gardé |
| `AddressSanitizer` introuvable | Composant non installé | Le cocher dans Visual Studio Installer |

---

## 🔗 Liens

- [[03 Premier programme]] — tester l'installation
- [[04 vcpkg — les dépendances]] — utiliser vcpkg dans un projet
- [[03 CMakePresets]] — ce que CLion lit
