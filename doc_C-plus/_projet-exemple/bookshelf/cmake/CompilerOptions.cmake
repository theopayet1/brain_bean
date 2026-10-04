# Options de compilation communes à toutes les cibles du projet.
# Chaque cible lie `bookshelf::options` en PRIVATE : rien ne fuit vers l'extérieur.

add_library(bookshelf_options INTERFACE)
add_library(bookshelf::options ALIAS bookshelf_options)

if(MSVC)
    target_compile_options(bookshelf_options INTERFACE
        /W4                 # beaucoup d'avertissements
        /permissive-        # respect strict de la norme
        /utf-8              # sources et chaînes en UTF-8 (les accents)
        /Zc:preprocessor    # préprocesseur conforme à la norme
        /EHsc               # exceptions C++
        /external:W0        # pas d'avertissement dans les en-têtes des bibliothèques
    )
    target_compile_definitions(bookshelf_options INTERFACE
        NOMINMAX            # windows.h ne définit plus les macros min et max
        WIN32_LEAN_AND_MEAN # windows.h plus léger
        UNICODE _UNICODE    # API Windows en UTF-16 (les fonctions ...W)
    )
    if(BOOKSHELF_WARNINGS_AS_ERRORS)
        target_compile_options(bookshelf_options INTERFACE /WX)
        target_link_options(bookshelf_options INTERFACE /WX)
    endif()
else()
    target_compile_options(bookshelf_options INTERFACE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        # Les initialisations désignées partielles (.title = …) sont voulues.
        -Wno-missing-field-initializers
    )
    if(BOOKSHELF_WARNINGS_AS_ERRORS)
        target_compile_options(bookshelf_options INTERFACE -Werror)
    endif()
endif()

if(BOOKSHELF_ASAN)
    if(MSVC)
        target_compile_options(bookshelf_options INTERFACE /fsanitize=address)
        # ASan n'accepte pas l'édition de liens incrémentale.
        target_link_options(bookshelf_options INTERFACE /INCREMENTAL:NO)
        # Les bibliothèques vcpkg ne sont pas compilées avec ASan : sans ces deux
        # définitions, l'éditeur de liens refuse le mélange.
        target_compile_definitions(bookshelf_options INTERFACE
            _DISABLE_VECTOR_ANNOTATION
            _DISABLE_STRING_ANNOTATION
        )
    else()
        target_compile_options(bookshelf_options INTERFACE
            -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(bookshelf_options INTERFACE -fsanitize=address,undefined)
    endif()
endif()

if(BOOKSHELF_CLANG_TIDY)
    find_program(BOOKSHELF_CLANG_TIDY_EXE clang-tidy REQUIRED)
    set(CMAKE_CXX_CLANG_TIDY "${BOOKSHELF_CLANG_TIDY_EXE}")
    if(MSVC)
        # Sans /EHsc, clang-tidy croit les exceptions désactivées et refuse chaque try.
        list(APPEND CMAKE_CXX_CLANG_TIDY "--extra-arg=/EHsc")
    endif()
endif()
