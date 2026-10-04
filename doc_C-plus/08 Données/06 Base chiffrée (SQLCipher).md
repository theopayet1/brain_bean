---
tags:
  - projet/cpp
  - type/guide
  - techno/sqlcipher
  - techno/sqlite
  - sujet/base-de-donnees
  - sujet/securite
  - statut/a-jour
aliases:
  - SQLCipher
  - Chiffrer SQLite
cree: 2026-10-04
maj: 2026-10-04
---

# Base chiffrée (SQLCipher)

> [!abstract] En une phrase
> Si les données sont sensibles, le fichier `.db` ne doit pas être lisible par quiconque copie le disque. **SQLCipher** est une version de SQLite qui **chiffre tout le fichier** (AES-256) avec une clé dérivée d'un **mot de passe**. Même API que SQLite, plus quelques fonctions : poser la clé **avant toute lecture**, puis vérifier qu'elle est bonne.

---

## 📦 L'installer

`vcpkg.json` : remplacer `sqlite3` par `sqlcipher`.

```cmake
find_package(sqlcipher CONFIG REQUIRED)
target_link_libraries(bookshelf_infrastructure PRIVATE sqlcipher::sqlcipher)
```

Un seul fichier inclut l'en-tête, avec la macro qui active les fonctions de chiffrement :

```cpp
// infrastructure/sqlite/sqlcipher.hpp
#pragma once

// SQLITE_HAS_CODEC doit être défini AVANT l'en-tête, sinon sqlite3_key_v2 n'est pas déclarée.
#ifndef SQLITE_HAS_CODEC
#define SQLITE_HAS_CODEC 1
#endif

#include <sqlcipher/sqlite3.h>
```

Tous les `.cpp` qui incluaient `<sqlite3.h>` incluent `"infrastructure/sqlite/sqlcipher.hpp"`.

---

## 🔑 Ouvrir avec un mot de passe

Ajouter d'abord à `Connection` un accès au pointeur brut, nécessaire pour les fonctions de SQLCipher :

```cpp
// Pointeur brut, pour poser la clé de chiffrement.
[[nodiscard]] sqlite3* raw() const noexcept { return db_.get(); }
```

Et trois codes d'erreur dans `errors.hpp` : `PasswordEmpty`, `PasswordInvalid`, `DatabaseUnavailable`.

**L'ordre est strict** : ouvrir, poser la clé, régler, puis **lire** pour vérifier.

```cpp
domain::Result<void> unlock(Connection& connection, std::string_view password)
{
    // Une clé vide désactiverait le chiffrement sans rien dire.
    if (password.empty())
    {
        return domain::fail(errors::PasswordEmpty);
    }

    // La clé passe par l'API, JAMAIS par « PRAGMA key = '...' » construit par concaténation :
    // une apostrophe dans le mot de passe casserait la requête.
    if (sqlite3_key_v2(connection.raw(), "main", password.data(),
                       static_cast<int>(password.size())) != SQLITE_OK)
    {
        return domain::fail(errors::DatabaseUnavailable, "sqlite3_key_v2");
    }

    try
    {
        connection.execute("PRAGMA cipher_memory_security = ON;");   // efface la clé de la RAM après usage
        connection.execute("PRAGMA foreign_keys = ON;");
        connection.execute("PRAGMA journal_mode = WAL;");
        connection.execute("PRAGMA secure_delete = ON;");            // une ligne supprimée est écrasée

        // Poser une mauvaise clé ne dit RIEN. Seule une vraie lecture révèle l'erreur.
        connection.execute("SELECT count(*) FROM sqlite_master;");
    }
    catch (const SqliteError& error)
    {
        if (error.code() == SQLITE_NOTADB)
        {
            return domain::fail(errors::PasswordInvalid);   // mauvais mot de passe
        }
        return domain::fail(errors::DatabaseUnavailable, error.what());
    }
    return {};
}
```

| Point | Pourquoi |
|---|---|
| Aucune requête **avant** `sqlite3_key_v2` | Sinon SQLCipher lit l'en-tête sans clé et échoue. Notre `Connection` exécute `PRAGMA foreign_keys` dans son constructeur : pour une base chiffrée, retirer cette ligne du constructeur et la faire ici, **après** la clé |
| `SQLITE_NOTADB` | Le code renvoyé par SQLCipher quand le mot de passe est faux |
| `cipher_memory_security` | La clé et les pages déchiffrées sont effacées de la mémoire dès qu'elles ne servent plus |
| `secure_delete` | Supprimer = effacer réellement du fichier |

## 🔁 Changer le mot de passe

```cpp
sqlite3_rekey_v2(connection.raw(), "main", newPassword.data(), static_cast<int>(newPassword.size()));
```

Exiger **l'ancien** mot de passe avant (le vérifier comme à l'ouverture).

---

## 🛡️ Ce qui va avec

| Mesure | Pourquoi |
|---|---|
| Le mot de passe dans une classe qui **efface sa mémoire** à la destruction (`SecureZeroMemory`) | Il ne traîne pas en RAM |
| Jamais de mot de passe au journal, ni dans un message d'erreur | Voir [[06 Le pont côté C++]] |
| Verrouiller après inactivité : détruire la session (connexion, services) | La clé disparaît de la mémoire |
| Sauvegarde = `VACUUM INTO 'copie.db'` | La copie est chiffrée avec la même clé |
| Le profil WebView2 ne garde rien des formulaires | [[09 Sécuriser la webview]] |

> [!warning] Mot de passe perdu = données perdues
> Il n'existe **aucun** moyen de relire une base SQLCipher sans le mot de passe. Prévoir une politique de sauvegarde, et le dire clairement à l'utilisateur.

---

## 🔗 Liens

- [[01 SQLite — enveloppe RAII]] — la `Connection` à adapter
- [[09 Sécuriser la webview]] — la sécurité côté interface
- [[00 Tests C++]] — la suite
