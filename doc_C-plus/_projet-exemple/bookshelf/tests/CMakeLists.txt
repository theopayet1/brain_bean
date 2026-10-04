find_package(doctest CONFIG REQUIRED)

# Un exécutable de tests par couche : celui du domaine ne lie QUE le domaine, ce qui
# prouve qu'il se teste sans base ni fenêtre.
function(bookshelf_add_tests name)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "" "SOURCES;LIBRARIES")
    add_executable(${name} main.cpp ${arg_SOURCES})
    target_include_directories(${name} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(${name} PRIVATE bookshelf::options doctest::doctest ${arg_LIBRARIES})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

bookshelf_add_tests(tests_domain
    SOURCES
        domain/book_tests.cpp
        domain/dates_tests.cpp
    LIBRARIES
        bookshelf::domain
)

bookshelf_add_tests(tests_application
    SOURCES
        application/book_service_tests.cpp
    LIBRARIES
        bookshelf::application
)

bookshelf_add_tests(tests_infrastructure
    SOURCES
        infrastructure/books_sqlite_tests.cpp
    LIBRARIES
        bookshelf::infrastructure
)

bookshelf_add_tests(tests_bridge
    SOURCES
        bridge/bridge_tests.cpp
        bridge/worker_tests.cpp
    LIBRARIES
        bookshelf::bridge
)

# Vérifie qu'aucune couche n'inclut un en-tête d'une couche qui dépend d'elle.
add_test(
    NAME layers
    COMMAND "${CMAKE_COMMAND}" "-DSRC=${CMAKE_SOURCE_DIR}/src" -P "${CMAKE_SOURCE_DIR}/cmake/CheckLayers.cmake"
)

# Vérifie que le frontend embarqué ne référence aucune adresse externe.
if(BOOKSHELF_APP)
    add_test(
        NAME frontend_offline
        COMMAND "${CMAKE_COMMAND}" "-DDIST=${CMAKE_SOURCE_DIR}/frontend/dist" -P "${CMAKE_SOURCE_DIR}/cmake/CheckDist.cmake"
    )
endif()
