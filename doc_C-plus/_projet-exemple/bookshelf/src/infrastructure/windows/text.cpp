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
