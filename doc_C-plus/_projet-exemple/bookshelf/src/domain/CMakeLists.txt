# Domaine : les données et les règles pures. Aucune dépendance, STL seule.
add_library(bookshelf_domain STATIC
    book/book.cpp
    common/dates.cpp
    common/text.cpp
)
add_library(bookshelf::domain ALIAS bookshelf_domain)

target_include_directories(bookshelf_domain PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_domain PRIVATE bookshelf::options)
