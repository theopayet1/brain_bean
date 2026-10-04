cmake_minimum_required(VERSION 3.28)

project(bookshelf VERSION 0.1.0 LANGUAGES CXX)

# Le nom affiché (titre de fenêtre, dossier des données). Écrit à un seul endroit.
set(BOOKSHELF_APP_NAME "Bookshelf" CACHE STRING "Nom de l'application affiché à l'utilisateur")

option(BOOKSHELF_WARNINGS_AS_ERRORS "Un avertissement du compilateur arrête le build" ON)
option(BOOKSHELF_ASAN "Compile avec AddressSanitizer" OFF)
option(BOOKSHELF_CLANG_TIDY "Passe clang-tidy sur chaque fichier compilé" OFF)
option(BOOKSHELF_TESTS "Compile les tests" ON)
# La fenêtre (saucer + WebView2) n'existe que sous Windows. Ailleurs, on compile et on teste
# quand même tout le reste.
option(BOOKSHELF_APP "Compile l'application graphique" ${WIN32})

# Un seul standard pour tout le projet.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# compile_commands.json : utilisé par clang-tidy et les éditeurs.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Le projet n'utilise pas les modules C++ : inutile de les chercher dans chaque fichier.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

if(MSVC)
    # Bibliothèques vcpkg statiques : le runtime C++ l'est aussi. L'exe livré ne dépend
    # alors d'aucune DLL à installer.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CompilerOptions)
if(BOOKSHELF_APP)
    include(Frontend)
endif()

add_subdirectory(src)

if(BOOKSHELF_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
