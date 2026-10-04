# Règle d'architecture : une dépendance ne remonte jamais.
# Pour chaque couche : la liste des couches dont elle a le droit d'inclure les en-têtes.
# Usage : cmake -DSRC=<racine>/src -P CheckLayers.cmake

cmake_minimum_required(VERSION 3.28)

set(ALLOWED_domain "domain")
set(ALLOWED_application "domain;application")
set(ALLOWED_infrastructure "domain;application;infrastructure")
set(ALLOWED_bridge "domain;application;bridge")
set(ALLOWED_app "domain;application;infrastructure;bridge;app")
set(LAYERS domain application infrastructure bridge app)

set(violations "")
foreach(layer IN LISTS LAYERS)
    file(GLOB_RECURSE files "${SRC}/${layer}/*.hpp" "${SRC}/${layer}/*.cpp")
    foreach(file IN LISTS files)
        # Toutes les lignes #include "..." du fichier.
        file(STRINGS "${file}" includes REGEX "^[ \t]*#[ \t]*include[ \t]*\"")
        foreach(line IN LISTS includes)
            if(line MATCHES "#[ \t]*include[ \t]*\"([a-z_]+)/")
                set(target "${CMAKE_MATCH_1}")
                if(target IN_LIST LAYERS AND NOT target IN_LIST ALLOWED_${layer})
                    file(RELATIVE_PATH relative "${SRC}" "${file}")
                    string(APPEND violations "  ${relative} : la couche « ${layer} » inclut « ${target} »\n")
                endif()
            endif()
        endforeach()
    endforeach()
endforeach()

if(violations)
    message(FATAL_ERROR "Dépendances interdites entre couches :\n${violations}")
endif()
message(STATUS "Couches : aucune dépendance interdite.")
