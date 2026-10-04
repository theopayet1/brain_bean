---
tags:
  - projet/cpp
  - type/guide
  - techno/cmake
  - sujet/build
  - sujet/tests
  - statut/a-jour
aliases:
  - check.ps1
cree: 2026-10-04
maj: 2026-10-04
---

# Script de vérification avant commit

> [!abstract] En une phrase
> Un script PowerShell lance **tout** ce qui doit être vert avant un commit — format C++, vérifications du frontend, build debug avec AddressSanitizer, tous les tests — et s'arrête à la première erreur. Une commande, et on sait si on peut commiter.

---

## 📄 `scripts/check.ps1`

```powershell
# Tout vérifier avant un commit : format, frontend, build, tests.
#
#   .\scripts\check.ps1            format + frontend + build debug (AddressSanitizer) + tests
#   .\scripts\check.ps1 -Release   ajoute le build release et ses tests
#   .\scripts\check.ps1 -Format    reformate les sources au lieu de seulement vérifier
#
# À lancer depuis le « Developer PowerShell for VS 2022 », à la racine du projet.

param(
    [switch]$Release,
    [switch]$Format
)

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot

function Step([string]$title) {
    Write-Host ''
    Write-Host "== $title ==" -ForegroundColor Cyan
}

# Lance un programme et arrête tout s'il échoue.
function Run([string]$program, [string[]]$arguments) {
    & $program @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "Échec : $program $($arguments -join ' ')"
    }
}

Push-Location $root
try {
    Step 'Format C++'
    $sources = Get-ChildItem -Recurse -File -Path src, tests -Include *.cpp, *.hpp |
        ForEach-Object { $_.FullName }
    if ($Format) {
        Run 'clang-format' (@('-i') + $sources)
    }
    else {
        Run 'clang-format' (@('--dry-run', '--Werror') + $sources)
    }

    Step 'Frontend : types, lint, format, build'
    Push-Location frontend
    try {
        if (-not (Test-Path node_modules)) {
            Run 'npm.cmd' @('ci', '--no-audit', '--no-fund')
        }
        Run 'npm.cmd' @('run', 'check')
    }
    finally {
        Pop-Location
    }

    $presets = @('debug')
    if ($Release) { $presets += 'release' }
    foreach ($preset in $presets) {
        Step "Build $preset"
        Run 'cmake' @('--preset', $preset)
        Run 'cmake' @('--build', '--preset', $preset)
        Step "Tests $preset"
        Run 'ctest' @('--preset', $preset)
    }

    Write-Host ''
    Write-Host 'Tout est vert.' -ForegroundColor Green
}
finally {
    Pop-Location
}
```

| Étape | Ce qui est vérifié |
|---|---|
| Format C++ | `clang-format --dry-run --Werror` sur `src/` et `tests/` |
| Frontend | `npm run check` : types TypeScript, ESLint, Prettier, build Vite ([[03 Le frontend (Vite, TypeScript, Preact)]]) |
| Build debug | Compilation avec `/W4 /WX` et AddressSanitizer |
| Tests | Tous les exe de tests + le test des couches + « aucune adresse externe » |

```powershell
.\scripts\check.ps1              # avant chaque commit
.\scripts\check.ps1 -Format      # reformate d'abord
.\scripts\check.ps1 -Release     # avant de livrer
```

> [!warning] Encodage du script
> Windows PowerShell 5.1 lit un `.ps1` **sans BOM** comme de l'ANSI et abîme les accents. Le fichier est enregistré en **UTF-8 avec BOM** (règle dans `.editorconfig`).

---

## 🔗 Liens

- [[06 clang-format et clang-tidy]] — l'étape format
- [[00 Tests C++]] — l'étape tests
