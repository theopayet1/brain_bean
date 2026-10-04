# Une cible par couche. L'ordre suit le sens des dépendances : une couche ne lie que
# celles qui sont au-dessus d'elle dans ce fichier, jamais l'inverse.

# Racine d'inclusion commune : #include "domain/book/book.hpp".
set(BOOKSHELF_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(domain)
add_subdirectory(application)
add_subdirectory(infrastructure)
add_subdirectory(bridge)
if(BOOKSHELF_APP)
    add_subdirectory(app)
endif()
