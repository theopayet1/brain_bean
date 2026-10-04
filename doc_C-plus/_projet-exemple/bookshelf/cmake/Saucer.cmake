# Cible importée `saucer::saucer`.
#
# Le port vcpkg de saucer installe la bibliothèque et ses en-têtes, mais aucun fichier de
# configuration CMake : find_package(saucer) ne trouve rien. On reconstruit donc ici la cible,
# avec les dépendances que saucer déclare dans son propre CMakeLists.

set(_saucer_prefix "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

find_library(SAUCER_LIBRARY_RELEASE saucer PATHS "${_saucer_prefix}/lib" NO_DEFAULT_PATH REQUIRED)
find_library(SAUCER_LIBRARY_DEBUG saucer PATHS "${_saucer_prefix}/debug/lib" NO_DEFAULT_PATH REQUIRED)
find_path(SAUCER_INCLUDE_DIR saucer/webview.hpp PATHS "${_saucer_prefix}/include" NO_DEFAULT_PATH REQUIRED)

find_package(fmt CONFIG REQUIRED)
find_package(glaze CONFIG REQUIRED)
find_package(eraser CONFIG REQUIRED)
find_package(flagpp CONFIG REQUIRED)
find_package(lockpp CONFIG REQUIRED)
find_package(rebind CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS callable_traits)
find_package(unofficial-webview2 CONFIG REQUIRED)

add_library(saucer::saucer STATIC IMPORTED)
set_target_properties(saucer::saucer PROPERTIES
    IMPORTED_CONFIGURATIONS "DEBUG;RELEASE"
    IMPORTED_LOCATION_DEBUG "${SAUCER_LIBRARY_DEBUG}"
    IMPORTED_LOCATION_RELEASE "${SAUCER_LIBRARY_RELEASE}"
    MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
    MAP_IMPORTED_CONFIG_MINSIZEREL Release
)
target_include_directories(saucer::saucer INTERFACE "${SAUCER_INCLUDE_DIR}")
target_compile_definitions(saucer::saucer INTERFACE SAUCER_WEBVIEW2)
target_link_libraries(saucer::saucer INTERFACE
    Boost::callable_traits
    cr::eraser
    cr::flagpp
    cr::lockpp
    cr::rebind
    fmt::fmt
    glaze::glaze
    unofficial::webview2::webview2
    # Bibliothèques Windows dont dépend l'implémentation WebView2 de saucer.
    Dwmapi
    Shcore
    Shlwapi
    gdiplus
)

unset(_saucer_prefix)
