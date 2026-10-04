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
