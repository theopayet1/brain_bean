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
