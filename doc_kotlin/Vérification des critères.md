---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - sujet/evaluation
  - statut/a-jour
aliases:
  - Checklist des critères
  - Vérification critères
cours: P1
cree: 2026-10-27
maj: 2026-10-27
---

# Vérification des critères

> [!abstract] En une phrase
> Les **16 critères** de la grille, point par point, au niveau **« Maîtrisé »**. Chaque point a **le lien vers la note** (et la section *en italique*) qui t'explique comment le valider. Coche au fur et à mesure.

> [!info] Comment t'en servir
> - **Théorie (S)** : coche quand tu sais l'**expliquer à l'oral sans tes notes**, **schéma compris**.
> - **Pratique (SF) et Qualité (SE)** : coche quand c'est **dans ton code** (Metrix).
> - Pour les critères SF / SE, le niveau dépend du **nombre de points cochés** : 1 point = Non acquis, la moitié = En cours, presque tout = Acquis, **tout = Maîtrisé**.
> - La vue d'ensemble (barème, note à relire par critère) est dans [[Grille P1 Mobile]].

---

## 🧠 Théorie : ce que tu dois savoir expliquer

> [!tip] Ce que la grille répète pour « Maîtrisé »
> Des explications **claires**, qui vont **droit au but**, et savoir **schématiser**. Pour chaque critère, refais le schéma de la note **de tête**.

### S-5 · Les fondamentaux d'une app Android

> [!example] Diagnostic du 27/10
> Manifest : **en cours** · Cycle de vie : **acquis** · Rotation : **en cours** · Context : **acquis** · Intents : **maîtrisé** · Gradle : **maîtrisé**

- [ ] Dire ce qu'est le **manifest** et **ce qu'il contient** (nom, permissions, activities, activity principale) → [[01 Le manifest]] · *À quoi il sert*
- [ ] Expliquer que **c'est Android le gardien**, pas le manifest → [[03 Les Permissions]] · *Le gardien (Android)*
- [ ] Décrire l'**organisation des dossiers** (manifest, `java/`, `res/`, fichiers Gradle) → [[06 Organisation du projet]] · *Ce qu'on voit dans Android Studio*
- [ ] Dire à quoi sert **chaque fichier Gradle** (`settings`, `build.gradle.kts` racine et `app/`, `libs.versions.toml`) → [[01 Les fichiers Gradle]] · *La carte*
- [ ] Donner les **4 états** du cycle de vie (tes mots **et** `CREATED`, `STARTED`…) et les **callbacks par paires** → [[03 Le cycle de vie]] · *Les 4 états*
- [ ] Dire **quoi faire à quelle étape** (Koin dans `Application.onCreate`, capteurs dans `onResume` / `onPause`) → [[03 Le cycle de vie]] · *Quoi faire à quelle étape*
- [ ] Expliquer la **rotation** (activity **détruite puis recréée**) et ce qui **survit** (ViewModel, `rememberSaveable`) → [[03 Le cycle de vie]] · *La rotation*
- [ ] Définir le **Context** (le médiateur avec le système) et donner les **deux Contexts** (Application et Activity) → [[01 Le Context]] · *Les deux Contexts à connaître*
- [ ] Expliquer la **fuite mémoire** et pourquoi il n'y a **pas de Context dans un ViewModel** → [[01 Le Context]] · *Où trouver un Context*
- [ ] Intent **explicite** et **implicite**, `putExtra`, `intent-filter`, `PendingIntent` → [[02 Les Intents]]

### S-1 · La Clean Architecture

> [!example] Diagnostic du 27/10
> **Maîtrisé**, à condition de dessiner la flèche Data → Domain dans le bon sens.

> [!warning] Erreur dans l'Excel
> Pour S-1, la colonne « Maîtrisé » est un **copier-coller de S-5** (manifest, Gradle…). Les points ci-dessous viennent de la **description** du critère.

- [ ] Nommer les **3 couches** et le **rôle** de chacune (UI affiche, Domain = le métier, Data = les données) → [[00 Architecture]] · *Les 3 couches*
- [ ] Dessiner **`UI → Domain ← Data`** : tout pointe vers le Domain, qui ne dépend de **rien** → [[00 Architecture]]
- [ ] Expliquer **comment** Data dépend du Domain : interface dans Domain, implémentation dans Data, Koin branche → [[00 Architecture]] · *Comment les couches se branchent*
- [ ] Donner les **avantages** : indépendance des frameworks, testabilité, maintenance, travail en équipe → [[00 Architecture]] · *Pourquoi s'embêter*
- [ ] Faire le lien avec ta version du cours, **Screen / ViewModel / Model** → [[01 Architecture (Screen - ViewModel - Model)]]

### S-3 · La couche Domain

> [!example] Diagnostic du 27/10
> **Acquis** : il manque les **arguments concrets** du « pourquoi Kotlin pur ».

- [ ] Dire ce qu'il y a dedans : **Domain Models**, **interfaces Repository**, use cases → [[02 La couche Domain]] · *Ce qu'il y a dedans*
- [ ] Expliquer la différence **modèle du Domain ≠ DTO** → [[02 La couche Domain]] · *Modèle du Domain ≠ DTO*
- [ ] Expliquer le **contrat** : le Domain veut un résultat, il ne sait pas **d'où** il vient → [[02 La couche Domain]] · *Les interfaces Repository*
- [ ] Justifier le **Kotlin pur** : testable sans émulateur, indépendant des frameworks, réutilisable (= **scalabilité** et **maintenabilité**) → [[00 Architecture]] · *Pourquoi s'embêter*

### S-2 · La couche Data

> [!example] Diagnostic du 27/10
> Les couples : **acquis** · Le fetch : **en cours** (il manquait l'écriture dans Room).

- [ ] Définir **Service / DTO** (la source distante) et **DAO / Entity** (la source locale) → [[03 La couche Data]] · *Les deux couples*
- [ ] Expliquer le **mapping** vers le **modèle du Domain** (`toDomain()`) et **pourquoi** (l'API change, le Domain ne voit rien) → [[03 La couche Data]] · *Le mapping*
- [ ] Expliquer le **mécanisme de fetch offline-first** : Room = seule source de vérité, l'API rafraîchit Room, le `Flow` prévient l'écran → [[03 La couche Data]] · *Le mécanisme de fetch*
- [ ] Dire **ce qui transite** entre les couches (que des modèles du Domain) → [[03 La couche Data]] · *Ce qui transite entre les couches*
- [ ] **Schématiser** le trajet complet d'une donnée → [[00 Architecture]] · *Le trajet complet d'une donnée*

### S-4 · La couche UI et Jetpack Compose

> [!example] Diagnostic du 27/10
> MVI / UDF : **maîtrisé** · Navigation : **acquis** (rôles NavController / NavHost à préciser).

- [ ] Expliquer l'**UI déclarative** (on décrit l'écran) et la **recomposition** → [[01 UI déclarative]], [[00 Compose]] · *La recomposition*
- [ ] Expliquer **UDF** : l'état descend, les actions remontent, et pourquoi on trouve les bugs plus vite → [[04 La couche UI (MVI et UDF)]] · *UDF*
- [ ] Décomposer **MVI** et ne pas confondre l'Intent de MVI avec l'**Intent Android** → [[04 La couche UI (MVI et UDF)]] · *MVI*
- [ ] Expliquer la **gestion d'état** : un `UiState` immuable dans un `StateFlow`, `copy()`, `collectAsStateWithLifecycle()` → [[04 La couche UI (MVI et UDF)]] · *L'état (UiState)*
- [ ] Expliquer **Single Activity** et la navigation : **NavController** (le chauffeur, la pile) et **NavHost** (la carte des routes) → [[04 MainActivity]] · *Étape 5*
- [ ] Expliquer le **découpage en Lego** (Screen → Content → composants) → [[04 La couche UI (MVI et UDF)]] · *Découper en Lego*

### S-6 · Les capacités natives

> [!example] Diagnostic du 27/10
> Permissions : **acquis** · Qui fait quoi (Screen, ViewModel, Manager, Context) : **en cours**.

- [ ] Expliquer le rôle du **Context** comme **médiateur** avec les services système (`getSystemService`) → [[01 Le Context]] · *Le Context = le guichet du système*
- [ ] Permissions **normales** (pas de popup, ex. `VIBRATE`) et **dangereuses** (popup, ex. caméra, GPS) → [[03 Les Permissions]] · *Deux types de permissions*
- [ ] Expliquer **pourquoi un Manager** : pas de matériel dans l'écran, réutilisable, testable → [[04 Les capacités natives (Managers)]] · *Le problème*
- [ ] Dessiner **qui fait quoi**, avec les couches : Screen → ViewModel (quand) → Manager (comment) → Context → matériel → [[04 Les capacités natives (Managers)]] · *La solution*
- [ ] Raconter un exemple complet de A à Z → [[4.1 Le vibreur pas à pas]], [[4.2 La localisation pas à pas]]

---

## 🛠️ Pratique : ce que ton code doit montrer

### SF-5 · Les fondamentaux dans le projet

- [ ] Manifest **propre**, **zéro warning** → [[05 KDoc et commentaires]] · *Commenter le manifest*
- [ ] Gradle **lisible**, **zéro warning** (attention à `kotlinOptions`) → [[06 Conventions de nommage et Lint]] · *Remplacer kotlinOptions*
- [ ] Versions des dépendances **centralisées** dans `libs.versions.toml` → [[01 Les fichiers Gradle]] · *Le catalogue*, [[02 Ajouter une dépendance]]
- [ ] **Aucun texte en dur** : tout passe par `stringResource` → [[4.1 Les ressources en pratique]] · *Utiliser une ressource*
- [ ] **Au moins une variante** de ressource : langue (`values-en/`) **ou** mode nuit (`values-night/`) → [[4.1 Les ressources en pratique]] · *Créer une variante*
- [ ] Une utilisation **utile** du cycle de vie, **avec un commentaire spécifique** → [[03 Le cycle de vie]] · *En Compose*, *Logger pour logger*
- [ ] Une **fonction d'extension** sur `ViewModel`, `Context` ou `Intent` → [[04 Les fonctions d'extension]]

### SF-1 · L'architecture en couches

- [ ] Packages `ui/`, `domain/`, `data/`, `di/` → [[06 Organisation du projet]] · *Ranger le code en packages*
- [ ] Les couches sont **séparées** : l'UI n'importe jamais rien de `data/` → [[00 Architecture]]
- [ ] **Koin** branche tout : `single`, `viewModelOf`, `androidContext()` → [[05 Koin (injection de dépendances)]] · *Comment on s'en sert*
- [ ] Les dépendances passent par **Gradle** et le catalogue → [[02 Ajouter une dépendance]] · *Exemple complet : ajouter Koin*

### SF-3 · Les composants Domain

- [ ] Des **modèles métier** en Kotlin pur (aucun import `android.*`) → [[02 La couche Domain]] · *Les Domain Models*
- [ ] Des **interfaces Repository** indépendantes des couches externes → [[02 La couche Domain]] · *Les interfaces Repository*
- [ ] Les ViewModels dépendent des **interfaces**, pas des implémentations → [[00 Architecture]] · *Comment les couches se branchent*

### SF-4 · Les composants Data

- [ ] Couples **DAO / Entity** et **Service / DTO** bien séparés → [[03 La couche Data]] · *Les deux couples*
- [ ] Un mécanisme de **fetch local / distant cohérent** → [[03 La couche Data]] · *Le mécanisme de fetch*
- [ ] Des **mappers** écrits (`toDomain()`, `toEntity()`) → [[03 La couche Data]] · *Le mapping*
- [ ] Code **modulaire**, qui respecte la Clean Archi → [[00 Architecture]]

### SF-6 · Le matériel

- [ ] **Au moins 2 capacités natives** (son, BDD, GPS, gyroscope, vibreur…) → [[04 Les capacités natives (Managers)]] · *Les capacités natives*
- [ ] Chacune dans un **Manager**, **aucun** `getSystemService` dans un composable → [[4.1 Le vibreur pas à pas]], [[4.2 La localisation pas à pas]]
- [ ] Les permissions **dangereuses** demandées à l'exécution (popup) → [[4.2 La localisation pas à pas]] · *L'écran*
- [ ] Une **fonction d'extension sur `Context`** → [[04 Les fonctions d'extension]] · *Sur Context*

---

## ✨ Qualité : l'expérience du développeur

### SE-1 · La couche Data

- [ ] Fonctions et services **commentés** en KDoc → [[05 KDoc et commentaires]] · *La structure d'un KDoc*
- [ ] Fonctions **découpées** et **réutilisables**, **pas de duplication** → [[03 La couche Data]]
- [ ] Noms de fonctions et de variables **explicites** → [[06 Conventions de nommage et Lint]] · *Les conventions Kotlin*
- [ ] Couples DAO / Entity et Service / DTO **bien définis et séparés** → [[03 La couche Data]] · *Les deux couples*
- [ ] **Headers KDoc** sur les composants complexes → [[05 KDoc et commentaires]] · *Les block tags*

### SE-2 · La couche UI

- [ ] Un nouvel écran **ressemble aux autres** (même dossier, mêmes noms, même découpage) → [[06 Organisation du projet]]
- [ ] Écrans et fonctionnalités **bien nommés** (`HostListScreen`, `HostListViewModel`) → [[06 Conventions de nommage et Lint]] · *Les suffixes d'une app Android*
- [ ] Composants **au bon endroit** (`ui/core/components`, `ui/screens/…`) → [[06 Organisation du projet]]
- [ ] Composables découpés en **Lego** → [[04 La couche UI (MVI et UDF)]] · *Découper en Lego*, [[02 Les composants]]
- [ ] Actions du ViewModel aux **noms clairs** (`onVibrateClick`, `onPermissionResult`) → [[04 La couche UI (MVI et UDF)]] · *Le ViewModel*
- [ ] Un **`UiState`** bien formé, qui décrit ce qui est **affiché** → [[04 La couche UI (MVI et UDF)]] · *L'état (UiState)*
- [ ] **Un composable complexe** avec un header KDoc et des block tags → [[05 KDoc et commentaires]] · *KDoc d'un composable complexe*

### SE-3 · La couche Domain

- [ ] Responsabilités métier **isolées**, composants **réutilisables et indépendants** → [[02 La couche Domain]]
- [ ] **Headers KDoc** qui expliquent la logique métier sur les composants complexes → [[05 KDoc et commentaires]], [[02 La couche Domain]] · *Ce que la grille attend*
- [ ] Interfaces **clairement définies**, indépendantes des couches externes → [[02 La couche Domain]] · *Les interfaces Repository*

### SE-4 · Le projet

- [ ] Manifest **sans warning**, avec des **commentaires** (un par permission et par activity) → [[05 KDoc et commentaires]] · *Commenter le manifest*
- [ ] Gradle **sans warning**, avec des **sections** (commentaires ou regions) → [[05 KDoc et commentaires]] · *Commenter Gradle*
- [ ] Versions **faciles à trouver**, **noms pertinents** dans le catalogue → [[01 Les fichiers Gradle]] · *Le catalogue*
- [ ] Ressources nommées selon **une convention** (`ic_`, `écran_élément_rôle`…) → [[4.1 Les ressources en pratique]] · *Nommer les ressources*
- [ ] Le commentaire du **cycle de vie** est clair et dit **pourquoi** → [[03 Le cycle de vie]] · *En Compose*
- [ ] L'**extension** a un header KDoc avec des **block tags** (`@receiver`, `@param`, `@return`) → [[04 Les fonctions d'extension]], [[05 KDoc et commentaires]] · *Les block tags*

### SE-5 · La qualité de code

- [ ] Des **Managers** pour accéder au matériel → [[04 Les capacités natives (Managers)]]
- [ ] Conventions Kotlin respectées : **Code → Inspect Code**, zéro warning de nommage → [[06 Conventions de nommage et Lint]] · *Lint : trouver les warnings*
- [ ] Les Managers ont des **commentaires clairs** sur leur fonctionnement → [[4.1 Le vibreur pas à pas]] · *Le Manager*
- [ ] L'**extension du Context** a un header KDoc avec des block tags → [[04 Les fonctions d'extension]]
- [ ] **Une seule langue** pour tous les commentaires → [[05 KDoc et commentaires]] · *Ce qu'il ne faut pas écrire*

---

## 🔗 Liens

- [[Grille P1 Mobile]] — le barème et la note à relire par critère
- [[kotlin]] — accueil du cours
