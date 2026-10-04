# Bookshelf — projet modèle

Une petite application de bureau Windows : un gestionnaire de livres. Elle sert de **modèle
complet** à copier pour démarrer une nouvelle application C++ avec une interface web.

- Cœur en **C++23**, découpé en couches : `domain` → `application` → `infrastructure` / `bridge` → `app`.
- Fenêtre **saucer** (WebView2), interface **Preact + TypeScript** construite par **Vite** et
  embarquée dans l'exe.
- Base **SQLite**, JSON **glaze**, tests **doctest**, dépendances **vcpkg**.

Toutes les explications sont dans le vault (note `cpp.md`, section « Tutoriel »).

## Prérequis

Visual Studio 2022 (charge « Développement Desktop en C++ »), vcpkg avec `VCPKG_ROOT` défini,
Node.js 22+.

## Compiler et lancer

Depuis le « Developer PowerShell for VS 2022 », à la racine :

```powershell
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
.\build\debug\src\app\Bookshelf.exe
```

Avant chaque commit : `.\scripts\check.ps1`.

Travailler l'interface seule, dans un navigateur : `cd frontend`, `npm run dev`.

## Structure

```
src/
  domain/          données et règles pures — STL seule
  application/     cas d'usage (services) et interfaces (ports) — STL seule
  infrastructure/  SQLite, fichiers, horloge, API Windows
  bridge/          le pont JSON entre l'interface et les services, le fil de travail
  app/             main, fenêtre, frontend embarqué — l'assemblage
frontend/          interface (Vite + TypeScript + Preact)
tests/             un exécutable de tests par couche
cmake/  scripts/
```
