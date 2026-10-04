---
tags:
  - projet/cpp
  - type/archi
  - techno/cpp
  - techno/sqlite
  - sujet/architecture
  - statut/a-jour
aliases:
  - Couche infrastructure
cree: 2026-10-04
maj: 2026-10-04
---

# L'infrastructure

> [!abstract] En une phrase
> L'**infrastructure** contient tout ce qui touche au **monde réel** : la base SQLite, les fichiers, l'horloge du système, les API Windows. Chaque classe y **implémente un port** de l'application. C'est la seule couche qui inclut `<sqlite3.h>` ou `<windows.h>`.

---

## 📂 Organisation

```text
src/infrastructure/
├─ CMakeLists.txt
├─ sqlite/
│  ├─ connection.hpp/.cpp    Connection, Statement, Transaction (RAII sur l'API C)
│  ├─ migrations.hpp/.cpp    le schéma, versionné
│  └─ books_sqlite.hpp/.cpp  BooksSqlite : le port IBooks
├─ system/
│  ├─ system_clock.hpp/.cpp  SystemClock : le port IClock
│  └─ file_log.hpp/.cpp      FileLog : le journal des erreurs
└─ windows/                  compilé seulement sous Windows
   ├─ data_folder.hpp/.cpp   %LOCALAPPDATA%\<appli>
   └─ text.hpp/.cpp          UTF-8 ⇄ UTF-16
```

Un sous-dossier par **technologie**. Nommage : `<Chose><Techno>` → `BooksSqlite`, `SystemClock`.

---

## ⏰ L'horloge système

```cpp
#pragma once

#include "application/ports/clock.hpp"

namespace bookshelf::infrastructure
{

// La vraie horloge : la date du jour à l'heure locale du poste.
class SystemClock : public application::IClock
{
public:
    [[nodiscard]] std::chrono::year_month_day today() const override;
};

} // namespace bookshelf::infrastructure
```

```cpp
#include "infrastructure/system/system_clock.hpp"

namespace bookshelf::infrastructure
{

std::chrono::year_month_day SystemClock::today() const
{
    // system_clock est en UTC : à 0 h 30 en France, la date UTC est encore la veille.
    // On passe donc par le fuseau du poste.
    const auto local = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    return std::chrono::year_month_day{std::chrono::floor<std::chrono::days>(local)};
}

} // namespace bookshelf::infrastructure
```

---

## 📓 Le journal

```cpp
#pragma once

#include <filesystem>
#include <mutex>
#include <string_view>

namespace bookshelf::infrastructure
{

// Le journal des erreurs techniques : une ligne horodatée par erreur, dans un fichier texte.
// C'est là qu'on regarde quand l'utilisateur dit « ça a affiché erreur inattendue ».
class FileLog
{
public:
    explicit FileLog(std::filesystem::path file);

    // Ne lève jamais : un journal qui plante ferait plus de dégâts que l'erreur elle-même.
    void error(std::string_view where, std::string_view detail) noexcept;

private:
    std::filesystem::path file_;
    std::mutex mutex_;
};

} // namespace bookshelf::infrastructure
```

```cpp
#include "infrastructure/system/file_log.hpp"

#include <chrono>
#include <format>
#include <fstream>
#include <utility>

namespace bookshelf::infrastructure
{

FileLog::FileLog(std::filesystem::path file)
    : file_(std::move(file))
{
}

void FileLog::error(std::string_view where, std::string_view detail) noexcept
{
    try
    {
        const std::scoped_lock lock{mutex_};
        std::ofstream out{file_, std::ios::app}; // ajoute à la fin, crée si absent
        const auto now = std::chrono::floor<std::chrono::seconds>(std::chrono::system_clock::now());
        out << std::format("{:%F %T} UTC [{}] {}\n", now, where, detail);
    }
    // NOLINTNEXTLINE(bugprone-empty-catch) : rien de mieux à faire si le disque refuse
    catch (...)
    {
    }
}

} // namespace bookshelf::infrastructure
```

| Point | Pourquoi |
|---|---|
| `noexcept` + `catch (...)` | Un journal qui lève pendant qu'on traite une erreur aggraverait tout |
| `std::mutex` | Peut être appelé depuis plusieurs fils |
| `std::ios::app` | Ajoute à la fin, crée le fichier si besoin |

---

## 🪟 Windows : le dossier des données

```cpp
#include "infrastructure/windows/data_folder.hpp"

#include "infrastructure/windows/text.hpp"

#include <windows.h>

#include <knownfolders.h>
#include <shlobj.h>

#include <memory>
#include <system_error>

namespace bookshelf::infrastructure
{

namespace
{

// Windows alloue le chemin avec CoTaskMemAlloc : il doit être rendu avec CoTaskMemFree.
struct CoTaskFree
{
    void operator()(wchar_t* text) const noexcept { CoTaskMemFree(text); }
};

} // namespace

std::optional<std::filesystem::path> dataFolder(std::string_view appName)
{
    PWSTR raw = nullptr;
    const HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &raw);
    // Confié tout de suite à un unique_ptr : libéré quoi qu'il arrive (RAII).
    const std::unique_ptr<wchar_t, CoTaskFree> owned{raw};
    if (FAILED(result))
    {
        return std::nullopt;
    }

    auto folder = std::filesystem::path{owned.get()} / toUtf16(appName);
    std::error_code error;
    std::filesystem::create_directories(folder, error);
    if (error)
    {
        return std::nullopt;
    }
    return folder;
}

} // namespace bookshelf::infrastructure
```

| Point | Pourquoi |
|---|---|
| `%LOCALAPPDATA%` | Le dossier prévu par Windows pour les données d'une appli, par utilisateur, **inscriptible** (contrairement à `Program Files`) |
| `std::unique_ptr<wchar_t, CoTaskFree>` | Le chemin alloué par Windows est rendu **quoi qu'il arrive** : [[03 Pointeurs intelligents]] |
| `std::filesystem` | Créer les dossiers sans API Windows |

### UTF-8 ⇄ UTF-16

```cpp
#include "infrastructure/windows/text.hpp"

#include <windows.h>

namespace bookshelf::infrastructure
{

std::wstring toUtf16(std::string_view text)
{
    if (text.empty())
    {
        return {};
    }
    const auto length = static_cast<int>(text.size());
    // Premier appel : combien de caractères UTF-16 faut-il ?
    const int size = MultiByteToWideChar(CP_UTF8, 0, text.data(), length, nullptr, 0);
    if (size <= 0)
    {
        return {};
    }
    // Deuxième appel : la conversion elle-même, dans une chaîne de la bonne taille.
    std::wstring result(static_cast<std::size_t>(size), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.data(), length, result.data(), size);
    return result;
}

std::string toUtf8(std::wstring_view text)
{
    if (text.empty())
    {
        return {};
    }
    const auto length = static_cast<int>(text.size());
    const int size =
        WideCharToMultiByte(CP_UTF8, 0, text.data(), length, nullptr, 0, nullptr, nullptr);
    if (size <= 0)
    {
        return {};
    }
    std::string result(static_cast<std::size_t>(size), '\0');
    WideCharToMultiByte(CP_UTF8, 0, text.data(), length, result.data(), size, nullptr, nullptr);
    return result;
}

} // namespace bookshelf::infrastructure
```

> [!tip] Le motif « deux appels »
> Beaucoup d'API Windows s'appellent **deux fois** : la première avec un tampon nul pour connaître la taille nécessaire, la seconde pour remplir un tampon de cette taille.

---

## 🗄️ SQLite

Toute la partie base a sa section : [[00 Données en C++]].

---

## 🔗 Liens

- [[01 SQLite — enveloppe RAII]] — la couche SQLite en détail
- [[06 La racine de composition]] — la suite
