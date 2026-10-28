---
tags:
  - projet/kotlin
  - type/reference
  - techno/kotlin
  - techno/android
  - techno/gradle
  - sujet/documentation
  - sujet/qualite
  - statut/a-jour
cours: P1
cree: 2026-10-26
maj: 2026-10-26
---

# KDoc et commentaires

> [!abstract] En une phrase
> Un **commentaire `//`** explique **pourquoi** une ligne est là ; un **KDoc `/** */`** est la **fiche d'identité** d'une classe ou d'une fonction, avec des **block tags** (`@param`, `@return`…). La grille demande les deux, dans le code, le manifest **et** Gradle.

> [!note] Complété le 26 octobre 2026
> Note ajoutée à partir de la grille d'évaluation : les 5 critères « expérience de développement » (SE-1 à SE-5) notent les commentaires.

---

## 🧭 Deux sortes de commentaires

| Sorte | Syntaxe | Dit… | Où |
|---|---|---|---|
| **KDoc** (header) | `/** … */` **au-dessus** | **Ce que c'est** et **comment s'en servir** | Classes, fonctions publiques, composables complexes, extensions |
| **Commentaire de ligne** | `// …` dans le code | **Pourquoi** cette ligne (pas ce qu'elle fait) | Une ligne pas évidente, un choix de cycle de vie |

> [!tip] Le KDoc s'affiche partout
> Survole un nom avec la souris (ou **Ctrl + Q**) : Android Studio affiche son KDoc. C'est pour ça que la grille dit « l'expérience du développeur qui viendra derrière vous sera améliorée ».

---

## ✍️ La structure d'un KDoc

```kotlin
/**
 * Résumé en une phrase : ce que fait la fonction.
 *
 * Paragraphe facultatif : le contexte, le piège, la règle métier.
 * On peut faire des liens vers du code avec des crochets : [Host].
 *
 * @param nom à quoi sert ce paramètre.
 * @return ce que la fonction renvoie (et les cas particuliers : null, false…).
 */
```

> [!tip] Le générer
> Tape **`/**`** puis **Entrée** au-dessus d'une fonction : Android Studio écrit le squelette avec les `@param` et `@return`. Il ne reste qu'à remplir.

---

## 🏷️ Les block tags

| Tag | Pour | Exemple |
|---|---|---|
| `@param nom` | Un **paramètre** de fonction (ou du constructeur, s'il n'est pas `val`) | `@param permission la permission à vérifier` |
| `@property nom` | Une **propriété** déclarée dans le constructeur (`val` / `var`) | `@property id Host identifier.` |
| `@receiver` | L'objet **étendu** par une [[04 Les fonctions d'extension\|fonction d'extension]] | `@receiver le Context qui interroge le système` |
| `@return` | Ce qui est **renvoyé** | `@return null si l'hôte n'existe pas` |
| `@throws Type` | Une **exception** que la fonction peut lancer | `@throws IllegalArgumentException si …` |
| `@see` | Un **lien** vers une autre classe / fonction utile | `@see ContextCompat.checkSelfPermission` |
| `@sample` | Un **exemple** d'utilisation (une fonction qui montre l'appel) | `@sample com.diiage.metrix.samples.hostSample` |
| `@constructor` | Le **constructeur** principal | `@constructor crée un Manager prêt à l'emploi` |

> [!success] « Block tags pertinents »
> Pertinent = **le bon tag au bon endroit**. Une extension sans `@receiver`, ou une fonction qui peut lancer une exception sans `@throws`, ce n'est pas pertinent.

### Un vrai exemple complet : Metrix

`domain/model/UserRole.kt` :

```kotlin
/**
 * Converts the API's numeric role value to a domain role.
 *
 * @param value Numeric role value received from the API.
 * @return The corresponding role.
 * @throws IllegalArgumentException If the API returns an unknown value.
 */
fun fromApiValue(value: Int): UserRole =
    entries.firstOrNull { it.apiValue == value }
        ?: throw IllegalArgumentException("Unknown user role value: $value")
```

| Tag | Pourquoi il est pertinent |
|---|---|
| `@param value` | Dit **d'où** vient la valeur (l'API) |
| `@return` | Dit ce qu'on récupère |
| `@throws` | Prévient l'appelant : **valeur inconnue = crash** s'il ne l'attrape pas |

---

## 🧩 KDoc d'un composable complexe

La grille (SE-2) demande **un composant complexe** avec un header KDoc :

```kotlin
/**
 * Carte d'un hôte dans la liste : nom, état actuel et nombre d'alertes.
 *
 * La carte est **grisée** quand l'hôte est désactivé : elle reste cliquable
 * pour pouvoir le réactiver depuis le détail.
 *
 * @param host l'hôte à afficher.
 * @param onClick appelé avec l'identifiant de l'hôte quand on touche la carte.
 * @param modifier pour placer la carte depuis le parent (padding, largeur…).
 * @see HostListScreen
 */
@Composable
fun HostCard(
    host: Host,
    onClick: (Int) -> Unit,
    modifier: Modifier = Modifier,
)
```

---

## 📄 Commenter le manifest

Le manifest est en **XML** : les commentaires s'écrivent **`<!-- … -->`**. La grille demande un manifest **sans warning** avec des commentaires **explicatifs**.

```xml
<manifest xmlns:android="http://schemas.android.com/apk/res/android">

    <!-- Appels à l'API Metrix -->
    <uses-permission android:name="android.permission.INTERNET" />
    <!-- Vibration à l'arrivée d'une alerte critique (permission normale, pas de popup) -->
    <uses-permission android:name="android.permission.VIBRATE" />
    <!-- Notifications push, Android 13+ (permission dangereuse : demandée à l'exécution) -->
    <uses-permission android:name="android.permission.POST_NOTIFICATIONS" />

    <!-- MetrixApplication démarre Koin avant toute activity -->
    <application
        android:name=".MetrixApplication"
        android:label="@string/app_name">

        <!-- Single Activity : la seule activity, point d'entrée de l'app -->
        <activity
            android:name=".MainActivity"
            android:exported="true">
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
```

👉 Un commentaire **par permission** qui dit **pourquoi** l'app en a besoin : c'est exactement ce qu'un relecteur veut savoir.

---

## 🐘 Commenter Gradle : sections et régions

La grille demande un Gradle « avec des **sections clairement démarquées** par des commentaires ou des **regions** ». Une **region** est un bloc qu'Android Studio sait **plier / déplier** :

```kotlin
dependencies {
    //region AndroidX et Compose
    implementation(libs.androidx.core.ktx)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.compose.material3)
    //endregion

    //region Injection de dépendances (Koin)
    implementation(platform(libs.koin.bom))
    implementation(libs.koin.androidx.compose)
    //endregion

    //region Réseau (Ktor + kotlinx.serialization)
    implementation(platform(libs.ktor.bom))
    implementation(libs.ktor.client.core)
    implementation(libs.ktor.client.cio)
    //endregion

    //region Tests
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    //endregion
}
```

| Code | Pourquoi |
|---|---|
| `//region Titre` … `//endregion` | Crée un bloc **repliable** avec un titre (petite flèche dans la marge) |
| Une region par **famille** de libs | On voit d'un coup d'œil **ce que l'app utilise** |

> [!tip] Le catalogue aussi
> Dans `libs.versions.toml`, les commentaires commencent par **`#`**. Regroupe les lignes par famille avec un `# --- Koin ---` au-dessus.

---

## 🚫 Ce qu'il ne faut pas écrire

> [!failure] À éviter
> - Un commentaire qui **répète le code** : `i++ // on ajoute 1 à i`.
> - Du **code commenté** laissé « au cas où » : Git s'en souvient pour toi.
> - Un commentaire **faux** parce que le code a changé depuis.
> - **Mélanger les langues** : un KDoc en anglais à côté d'un KDoc en français. Choisis-en une pour tout le projet (le code doit être « **indifférencié** »).

---

## 🔗 Liens

- [[00 Code]]
- [[04 Les fonctions d'extension]] — le tag `@receiver`
- [[03 Le cycle de vie]] — le commentaire « spécifique » du cycle de vie
- [[06 Conventions de nommage et Lint]] — l'autre moitié de la qualité
