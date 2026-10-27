---
tags:
  - projet/kotlin
  - type/concept
  - techno/kotlin
  - techno/android
  - sujet/extension
  - sujet/context
  - statut/a-jour
cours: P1
cree: 2026-10-26
maj: 2026-10-26
---

# Les fonctions d'extension

> [!abstract] En une phrase
> Une **fonction d'extension** permet d'**ajouter une fonction à une classe qui n'est pas à toi** (`Context`, `String`, `Intent`…) **sans la modifier**. On l'appelle ensuite comme si elle avait toujours été là : `context.hasPermission(...)`.

> [!note] Complété le 26 octobre 2026
> Note ajoutée à partir de la grille d'évaluation : la fonction d'extension (sur `Context`, `ViewModel` ou `Intent`) **avec un KDoc** est demandée dans **4 critères**.

---

## 🧩 L'idée

> [!example] L'autocollant
> Tu ne peux pas ouvrir la classe `Context` d'Android pour y ajouter du code. Mais tu peux **coller un autocollant** dessus : une fonction qui **s'écrit à part**, et qui s'utilise **comme si** elle faisait partie de `Context`.

```kotlin
// Sans extension : une fonction "utilitaire" à qui on passe le Context
hasPermission(context, Manifest.permission.CAMERA)

// Avec extension : on l'appelle SUR le Context
context.hasPermission(Manifest.permission.CAMERA)
```

---

## ✍️ La syntaxe

```kotlin
fun String.initiales(): String =
    split(" ").joinToString("") { it.first().uppercase() }

"Ada Lovelace".initiales()   // "AL"
```

| Code | Pourquoi |
|---|---|
| `fun String.initiales()` | `String` devant le nom = la classe qu'on **étend**. On l'appelle le **receveur** (*receiver*) |
| `split(" ")` (sans rien devant) | Dans la fonction, **`this`** = l'objet sur lequel on l'appelle. `split` veut dire `this.split` |
| `"Ada Lovelace".initiales()` | On l'appelle **comme une vraie méthode** de `String` |

> [!warning] Ce qu'elle ne peut pas faire
> Une extension ne voit **que le public** de la classe : pas de membre `private`. Elle n'ajoute **rien** à la classe, c'est juste une **écriture plus pratique**.

---

## 📱 Sur `Context` (le cas demandé par la grille)

### Vérifier une permission

```kotlin
/**
 * Vérifie si l'utilisateur a déjà accordé une permission à l'app.
 *
 * À appeler **avant** d'utiliser une capacité protégée (GPS, caméra…) :
 * si le résultat est `false`, il faut d'abord demander la permission.
 *
 * @receiver le [Context] qui interroge le système (Activity ou Application).
 * @param permission la permission à vérifier, par exemple [Manifest.permission.CAMERA].
 * @return `true` si la permission est accordée, `false` sinon.
 * @see ContextCompat.checkSelfPermission
 */
fun Context.hasPermission(permission: String): Boolean =
    ContextCompat.checkSelfPermission(this, permission) == PackageManager.PERMISSION_GRANTED
```

| Code | Pourquoi |
|---|---|
| `fun Context.hasPermission(...)` | On étend `Context` : marche avec une Activity **et** l'Application ([[12 Le Context]]) |
| `ContextCompat.checkSelfPermission(this, …)` | `this` = **le Context** sur lequel on l'appelle |
| `== PERMISSION_GRANTED` | Transforme le code d'Android en un `Boolean` lisible |
| `@receiver` | Le **block tag** fait pour les extensions : il documente **l'objet étendu** → [[06 KDoc et commentaires]] |

Utilisation, dans un composable :

```kotlin
val context = LocalContext.current
if (!context.hasPermission(Manifest.permission.ACCESS_FINE_LOCATION)) {
    launcher.launch(Manifest.permission.ACCESS_FINE_LOCATION)
}
```

### Ouvrir une page web (Context + Intent)

```kotlin
/**
 * Ouvre [url] dans le navigateur du téléphone.
 *
 * @receiver le [Context] qui lance l'[Intent]. Utiliser celui d'une Activity
 * (`LocalContext.current`) : l'Application Context ne peut pas lancer d'activity seul.
 * @param url l'adresse complète, avec `https://`.
 * @return `false` si aucune app ne sait ouvrir une page web, `true` sinon.
 */
fun Context.openUrl(url: String): Boolean =
    try {
        startActivity(Intent(Intent.ACTION_VIEW, url.toUri()))
        true
    } catch (exception: ActivityNotFoundException) {
        Log.w("openUrl", "Aucune app pour ouvrir $url", exception)
        false
    }
```

| Code | Pourquoi |
|---|---|
| `startActivity(Intent(ACTION_VIEW, …))` | Un **Intent implicite** : Android choisit le navigateur → [[11 Les Intents]] |
| `try / catch ActivityNotFoundException` | Pas de navigateur = **pas de crash**, on renvoie `false` |

### Sur `Intent`

```kotlin
/**
 * Lit l'identifiant d'hôte glissé dans l'Intent par l'écran précédent.
 *
 * @receiver l'[Intent] reçu par l'activity.
 * @return l'identifiant, ou `null` s'il n'y en a pas.
 */
fun Intent.hostIdOrNull(): Int? =
    getIntExtra("hostId", -1).takeIf { it != -1 }
```

---

## 🧪 Exemples réels dans Metrix

Metrix a déjà des extensions, **mais pas sur `Context`** :

| Extension | Fichier | Rôle |
|---|---|---|
| `HttpClient.getBodyOrNull(path)` | `data/remote/HttpClientExtensions.kt` | Fait un `GET`, et transforme une erreur **404** en `null` |
| `HostDto.toDomain()` | `data/mapper/ApiDtoMapper.kt` | Les **mappers** → [[02 La couche Data]] |

```kotlin
/**
 * Reads a JSON body and maps only a 404 response to `null`.
 *
 * Other HTTP errors and network failures are propagated to the caller.
 */
internal suspend inline fun <reified T> HttpClient.getBodyOrNull(path: String): T? =
    try {
        get(path).body<T>()
    } catch (exception: ClientRequestException) {
        if (exception.response.status == HttpStatusCode.NotFound) {
            null
        } else {
            throw exception
        }
    }
```

> [!tip] Il manque quoi pour la grille ?
> Ce KDoc n'a **pas de block tags** (`@receiver`, `@param`, `@return`, `@throws`). La grille demande un header « mettant en avant des **block tags pertinents** ».

---

## 🗂️ Où les ranger

Un fichier **par classe étendue**, nommé `<Classe>Extensions.kt`, à côté de ceux qui s'en servent :

| Fichier | Exemple de place |
|---|---|
| `ContextExtensions.kt` | `ui/core/` ou `data/device/` (avec les Managers) |
| `HttpClientExtensions.kt` | `data/remote/` (c'est ce que fait Metrix) |

> [!failure] À éviter
> - Mettre de la **logique métier** dans une extension sur `Context` : elle va dans le Domain.
> - Faire des extensions **partout** : si une fonction ne sert qu'une fois, une fonction `private` suffit.

---

## 🔗 Liens

- [[00 Code]]
- [[12 Le Context]] — la classe qu'on étend le plus
- [[06 KDoc et commentaires]] — le header obligatoire
- [[13 Les capacités natives (Managers)]] — où l'extension sur `Context` sert
