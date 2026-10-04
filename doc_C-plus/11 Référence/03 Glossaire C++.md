---
tags:
  - projet/cpp
  - type/reference
  - techno/cpp
  - statut/a-jour
aliases:
  - Glossaire C++
  - Vocabulaire C++
cree: 2026-10-04
maj: 2026-10-04
---

# Glossaire C++

> [!abstract] En une phrase
> Tous les mots de cette doc, en une phrase chacun, avec la note où ils sont expliqués.

---

| Mot | Sens | Note |
|---|---|---|
| **Agrégat** | Struct simple sans constructeur écrit, initialisable avec `{.champ = ...}` | [[07 Structs et classes]] |
| **AddressSanitizer (ASan)** | Option de compilation qui arrête le programme à la première erreur mémoire | [[07 Déboguer et sanitizers]] |
| **Boucle d'événements** | La boucle qui traite clics et messages tant que la fenêtre est ouverte (`application->run()`) | [[02 saucer — ouvrir une fenêtre]] |
| **Cible (CMake)** | Ce que CMake fabrique : un exe ou une bibliothèque, avec ses propriétés | [[01 CMake — les bases]] |
| **Comportement indéfini (UB)** | Situation où la norme ne dit rien : le programme peut faire n'importe quoi | [[05 Durée de vie et pièges]] |
| **Concept** | Contrainte sur un paramètre de template (`std::integral T`) | [[11 Templates]] |
| **CSP** | En-tête qui limite ce qu'une page web peut charger | [[09 Sécuriser la webview]] |
| **Déclaration / définition** | « ça existe » (`.hpp`) / « voilà comment » (`.cpp`) | [[08 Fichiers hpp et cpp]] |
| **Déplacement (`std::move`)** | Transférer le contenu d'un objet sans copie | [[04 Copie et déplacement]] |
| **Domaine** | La couche des données et règles pures | [[02 Le domaine]] |
| **DTO** | Struct qui décrit exactement la forme d'un message JSON | [[06 Le pont côté C++]] |
| **Édition de liens** | Assembler les fichiers objets et bibliothèques en un exe | [[04 De la source à l'exe]] |
| **En-tête** (`.hpp`) | Fichier de déclarations, inclus par les autres | [[08 Fichiers hpp et cpp]] |
| **`std::expected`** | Une valeur **ou** une erreur | [[01 Gérer les erreurs (expected et exceptions)]] |
| **Faux** (*fake*) | Implémentation simplifiée d'un port pour les tests | [[02 Tester un service avec des faux]] |
| **Fil d'exécution** (*thread*) | Du code qui s'exécute en parallèle | [[05 Threads et file de tâches]] |
| **Fil de travail** | Le fil unique qui exécute les demandes de l'interface | [[08 Le fil de travail]] |
| **Hook** | Fonction `useXxx` de Preact (`useState`, `useEffect`) | [[10 Écrans et composants Preact]] |
| **Injection de dépendances** | Donner à un objet ce dont il a besoin par son constructeur | [[04 Les services (cas d'usage)]] |
| **Interface** | Classe abstraite aux méthodes `virtual ... = 0` | [[12 Héritage et interfaces]] |
| **Lambda** | Petite fonction écrite sur place | [[10 Lambdas]] |
| **Migration** | Changement numéroté du schéma de la base | [[02 Migrations de schéma]] |
| **Module (saucer)** | Classe attachée à la fenêtre qui reçoit les messages du JS | [[08 Le fil de travail]] |
| **`noexcept`** | Promesse de ne jamais lever d'exception | [[06 const, constexpr, noexcept et nodiscard]] |
| **`std::optional`** | Une valeur ou rien | [[09 enum, optional et variant]] |
| **Pile / tas** | Mémoire automatique des variables locales / mémoire allouée | [[01 Pile et tas]] |
| **Pont** (*bridge*) | La couche qui traduit JSON ⇄ appels de services | [[05 Le pont C++ JavaScript — le protocole]] |
| **Port** | Interface déclarée par l'application pour ce dont elle a besoin | [[03 Les ports (interfaces)]] |
| **Preset (CMake)** | Configuration nommée prête à l'emploi | [[03 CMakePresets]] |
| **Promesse** (JS) | Un résultat qui arrivera plus tard | [[07 Le pont côté TypeScript]] |
| **RAII** | Une ressource liée à la vie d'un objet : prise au constructeur, rendue au destructeur | [[02 RAII]] |
| **Racine de composition** | Le seul endroit (le `main`) qui assemble toutes les couches | [[06 La racine de composition]] |
| **Référence** (`T&`) | Un autre nom pour un objet existant | [[06 Références et pointeurs]] |
| **Référence pendante** | Référence vers un objet déjà détruit | [[05 Durée de vie et pièges]] |
| **Schéma personnalisé** | Une adresse maison (`app://`) servie par une fonction C++ | [[04 Embarquer le frontend dans l'exe]] |
| **Service** | Classe qui regroupe les cas d'usage d'un domaine fonctionnel | [[04 Les services (cas d'usage)]] |
| **STL** | La bibliothèque standard du C++ (`std::...`) | [[05 Conteneurs de la STL]] |
| **`std::string_view`** | Vue sur un texte, sans copie ni propriété | [[04 Chaînes de caractères]] |
| **Template** | Code générique pour plusieurs types | [[11 Templates]] |
| **Transaction** | Groupe d'écritures en « tout ou rien » | [[04 Transactions]] |
| **Triplet (vcpkg)** | Pour quelle plateforme et quel type de liaison compiler | [[04 vcpkg — les dépendances]] |
| **Type fort** | Type dédié qui empêche les confusions (`Id<Book>`) | [[02 Types forts]] |
| **Unité de traduction** | Un `.cpp` avec tout ce qu'il inclut, compilé seul | [[04 De la source à l'exe]] |
| **`std::variant`** | Une valeur parmi plusieurs types | [[09 enum, optional et variant]] |
| **`virtual` / `override`** | Méthode redéfinissable / qui redéfinit | [[12 Héritage et interfaces]] |
| **WebView2** | Le moteur web d'Edge intégré dans une fenêtre Windows | [[02 saucer — ouvrir une fenêtre]] |

---

## 🔗 Liens

- [[cpp]] — accueil du vault
- [[04 Antisèche C++]] — la syntaxe
