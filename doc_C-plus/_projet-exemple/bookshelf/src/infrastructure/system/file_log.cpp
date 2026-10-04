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
