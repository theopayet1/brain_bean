# Infrastructure : tout ce qui touche au disque et au système.
find_package(SQLite3 REQUIRED)
# CMake 4.1 a renommé la cible SQLite::SQLite3 en SQLite3::SQLite3 : on prend celle qui existe.
if(TARGET SQLite3::SQLite3)
    set(BOOKSHELF_SQLITE_TARGET SQLite3::SQLite3)
else()
    set(BOOKSHELF_SQLITE_TARGET SQLite::SQLite3)
endif()

add_library(bookshelf_infrastructure STATIC
    sqlite/books_sqlite.cpp
    sqlite/connection.cpp
    sqlite/migrations.cpp
    system/file_log.cpp
    system/system_clock.cpp
)
add_library(bookshelf::infrastructure ALIAS bookshelf_infrastructure)

# Ce qui n'existe que sous Windows.
if(WIN32)
    target_sources(bookshelf_infrastructure PRIVATE
        windows/data_folder.cpp
        windows/text.cpp
    )
    # Shell32 : SHGetKnownFolderPath. Ole32 : CoTaskMemFree.
    target_link_libraries(bookshelf_infrastructure PRIVATE Shell32 Ole32)
endif()

target_include_directories(bookshelf_infrastructure PUBLIC "${BOOKSHELF_SRC_DIR}")
target_link_libraries(bookshelf_infrastructure
    PUBLIC
        bookshelf::domain
        bookshelf::application
    PRIVATE
        bookshelf::options
        ${BOOKSHELF_SQLITE_TARGET}
)
