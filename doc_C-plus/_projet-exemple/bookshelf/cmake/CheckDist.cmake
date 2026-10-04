# Le frontend embarqué ne doit référencer AUCUNE adresse externe (CDN, police en ligne,
# outil de mesure) : une seule suffirait à rendre l'application lente ou cassée sans réseau.
# Usage : cmake -DDIST=<racine>/frontend/dist -P CheckDist.cmake

cmake_minimum_required(VERSION 3.28)

file(GLOB_RECURSE files "${DIST}/*.html" "${DIST}/*.js" "${DIST}/*.css" "${DIST}/*.svg" "${DIST}/*.json")
if(NOT files)
    message(FATAL_ERROR "Aucun fichier dans ${DIST} : le frontend n'a pas été construit.")
endif()

set(violations "")
foreach(file IN LISTS files)
    file(READ "${file}" content)
    # Les espaces de noms XML (SVG, XHTML) sont des identifiants, jamais téléchargés.
    string(REGEX REPLACE "https?://www\\.w3\\.org/[A-Za-z0-9/.#-]*" "" content "${content}")
    string(REGEX MATCHALL "(https?:)?//[A-Za-z0-9-]+\\.[A-Za-z][A-Za-z0-9./_-]*" urls "${content}")
    if(urls)
        list(REMOVE_DUPLICATES urls)
        file(RELATIVE_PATH relative "${DIST}" "${file}")
        string(APPEND violations "  ${relative} : ${urls}\n")
    endif()
endforeach()

if(violations)
    message(FATAL_ERROR "Adresses externes dans le frontend embarqué :\n${violations}")
endif()
message(STATUS "Frontend embarqué : aucune adresse externe.")
