# Application : les cas d'usage et les interfaces (ports) dont ils ont besoin.
# Ne connaît ni la base, ni le système, ni l'interface : STL seule.
add_library(bookshelf_application STATIC
    books/book_service.cpp
)
add_library(bookshelf::application ALIAS bookshelf_application)

target_include_directories(bookshelf_application PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_application
    PUBLIC bookshelf::domain
    PRIVATE bookshelf::options
)
