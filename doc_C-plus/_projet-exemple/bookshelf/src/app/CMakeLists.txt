# Racine de composition : main, fenêtre, assemblage des couches. Windows seulement.
include(Saucer)

configure_file(identity.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/generated/app/identity.hpp" @ONLY)

# WIN32 : application graphique, aucune console ne s'ouvre au lancement.
add_executable(bookshelf WIN32
    embedded_frontend.cpp
    main.cpp
    window.cpp
)

target_include_directories(bookshelf PRIVATE
    "${BOOKSHELF_SRC_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)
target_link_libraries(bookshelf PRIVATE
    bookshelf::options
    bookshelf::infrastructure
    bookshelf::bridge
    bookshelf::frontend
    saucer::saucer
)
set_target_properties(bookshelf PROPERTIES OUTPUT_NAME "${BOOKSHELF_APP_NAME}")
