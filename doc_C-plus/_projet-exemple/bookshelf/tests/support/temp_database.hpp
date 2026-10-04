#pragma once

#include <filesystem>
#include <random>
#include <string>

namespace bookshelf::tests
{

// Un fichier de base unique dans le dossier temporaire, supprimé à la fin du test.
class TempDatabase
{
public:
    TempDatabase()
        : path_(std::filesystem::temp_directory_path() /
                ("bookshelf-test-" + std::to_string(std::random_device{}()) + ".db"))
    {
    }

    ~TempDatabase()
    {
        std::error_code ignored;
        std::filesystem::remove(path_, ignored);
    }

    TempDatabase(const TempDatabase&) = delete;
    TempDatabase& operator=(const TempDatabase&) = delete;

    [[nodiscard]] const std::filesystem::path& path() const { return path_; }

private:
    std::filesystem::path path_;
};

} // namespace bookshelf::tests
