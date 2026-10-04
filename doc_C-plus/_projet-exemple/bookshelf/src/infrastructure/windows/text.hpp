#pragma once

#include <string>
#include <string_view>

namespace bookshelf::infrastructure
{

// Le programme travaille en UTF-8 ; les API Windows « ...W » veulent de l'UTF-16.
// On convertit aux bords, ici, et nulle part ailleurs.
[[nodiscard]] std::wstring toUtf16(std::string_view text);
[[nodiscard]] std::string toUtf8(std::wstring_view text);

} // namespace bookshelf::infrastructure
