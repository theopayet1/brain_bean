# Pont : traduit les messages JSON de l'interface en appels de services, et leurs
# résultats en JSON. Ne connaît ni la base ni la fenêtre.
find_package(glaze CONFIG REQUIRED)

add_library(bookshelf_bridge STATIC
    bridge.cpp
    messages.cpp
    worker.cpp
)
add_library(bookshelf::bridge ALIAS bookshelf_bridge)

target_include_directories(bookshelf_bridge PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_bridge
    PUBLIC
        bookshelf::application
        bookshelf::domain
    PRIVATE
        bookshelf::options
        glaze::glaze
)
