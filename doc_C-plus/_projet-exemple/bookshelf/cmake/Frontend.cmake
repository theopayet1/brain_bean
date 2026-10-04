# Construit le frontend (Vite) et l'embarque dans l'exe avec CMakeRC.
# Résultat : la cible `bookshelf::frontend`, à lier à l'exécutable.
#
# CMakeRC a besoin de la liste des fichiers dès la configuration. Le dist/ est donc construit
# une première fois ici s'il n'existe pas ; ensuite il est reconstruit par le build dès qu'une
# source du frontend change. Vite sort des noms de fichiers FIXES (sans empreinte) pour que
# cette liste ne bouge pas à chaque modification.

find_package(CMakeRC CONFIG REQUIRED)
find_program(BOOKSHELF_NPM NAMES npm.cmd npm REQUIRED)

set(BOOKSHELF_FRONTEND_DIR "${CMAKE_SOURCE_DIR}/frontend")
set(BOOKSHELF_FRONTEND_DIST "${BOOKSHELF_FRONTEND_DIR}/dist")

function(bookshelf_run_npm)
    execute_process(
        COMMAND "${BOOKSHELF_NPM}" ${ARGN}
        WORKING_DIRECTORY "${BOOKSHELF_FRONTEND_DIR}"
        RESULT_VARIABLE code
    )
    if(NOT code EQUAL 0)
        message(FATAL_ERROR "Échec de `npm ${ARGN}` dans ${BOOKSHELF_FRONTEND_DIR}")
    endif()
endfunction()

if(NOT EXISTS "${BOOKSHELF_FRONTEND_DIR}/node_modules")
    message(STATUS "Frontend : installation des dépendances npm")
    bookshelf_run_npm(ci --no-audit --no-fund)
endif()

if(NOT EXISTS "${BOOKSHELF_FRONTEND_DIST}/index.html")
    message(STATUS "Frontend : première construction du dist/")
    bookshelf_run_npm(run build)
endif()

file(GLOB_RECURSE BOOKSHELF_FRONTEND_SOURCES CONFIGURE_DEPENDS
    "${BOOKSHELF_FRONTEND_DIR}/src/*"
    "${BOOKSHELF_FRONTEND_DIR}/public/*"
)
list(APPEND BOOKSHELF_FRONTEND_SOURCES
    "${BOOKSHELF_FRONTEND_DIR}/index.html"
    "${BOOKSHELF_FRONTEND_DIR}/package.json"
    "${BOOKSHELF_FRONTEND_DIR}/package-lock.json"
    "${BOOKSHELF_FRONTEND_DIR}/vite.config.ts"
    "${BOOKSHELF_FRONTEND_DIR}/tsconfig.json"
)

file(GLOB_RECURSE BOOKSHELF_FRONTEND_DIST_FILES CONFIGURE_DEPENDS "${BOOKSHELF_FRONTEND_DIST}/*")

add_custom_command(
    OUTPUT ${BOOKSHELF_FRONTEND_DIST_FILES}
    COMMAND "${BOOKSHELF_NPM}" run build
    WORKING_DIRECTORY "${BOOKSHELF_FRONTEND_DIR}"
    DEPENDS ${BOOKSHELF_FRONTEND_SOURCES}
    COMMENT "Frontend : vite build"
    VERBATIM
)

cmrc_add_resource_library(bookshelf_frontend
    ALIAS bookshelf::frontend
    NAMESPACE frontend
    WHENCE "${BOOKSHELF_FRONTEND_DIST}"
    ${BOOKSHELF_FRONTEND_DIST_FILES}
)
# Code généré par CMakeRC : il n'a pas à suivre nos règles d'analyse.
set_target_properties(bookshelf_frontend PROPERTIES CXX_CLANG_TIDY "")
