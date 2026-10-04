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
