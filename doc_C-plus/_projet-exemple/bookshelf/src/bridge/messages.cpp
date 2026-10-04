#include "bridge/messages.hpp"

#include "domain/common/errors.hpp"

#include <array>
#include <utility>

namespace bookshelf::bridge
{

namespace errors = domain::errors;

std::string_view messageFor(std::string_view code)
{
    static constexpr auto Messages = std::to_array<std::pair<std::string_view, std::string_view>>({
        {errors::BookNotFound, "Ce livre n'existe plus."},
        {errors::BookTitleMissing, "Le titre est obligatoire."},
        {errors::BookTitleTooLong, "Le titre est trop long (200 caractères au plus)."},
        {errors::BookYearInvalid, "L'année doit être comprise entre 1400 et 2100."},
    });
    for (const auto& [known, message] : Messages)
    {
        if (known == code)
        {
            return message;
        }
    }
    return "Une erreur inattendue s'est produite.";
}

} // namespace bookshelf::bridge
