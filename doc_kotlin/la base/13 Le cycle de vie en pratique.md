---
tags:
  - projet/kotlin
  - type/guide
  - techno/kotlin
  - techno/android
  - techno/compose
  - sujet/cycle-de-vie
  - sujet/activity
  - statut/a-jour
cours: P1
cree: 2026-10-26
maj: 2026-10-26
---

# Le cycle de vie en pratique

> [!abstract] En une phrase
> Connaître les étapes du cycle de vie, c'est la théorie ([[02 Les Activities#Le cycle de vie d'une Activity]]). Ici, on voit **quoi faire à quelle étape**, en Activity **et** en Compose, et **comment le commenter**.

> [!note] Complété le 26 octobre 2026
> Note ajoutée à partir de la grille d'évaluation : « utilisation **judicieuse** du cycle de vie, mise en avant par un **commentaire spécifique** ». Pas tirée de tes notes de cours.

---

## 🗣️ Tes mots et les mots officiels

À l'oral, donne **les deux** : tes mots montrent que tu as compris, les mots officiels montrent que tu connais Android.

| Tes mots | État officiel (`Lifecycle.State`) | On y arrive par |
|---|---|---|
| **mort** | `DESTROYED` | `onDestroy` |
| **purgatoire** | `CREATED` | `onCreate` (en entrant) ou `onStop` (en sortant) |
| **visible** | `STARTED` | `onStart` (en entrant) ou `onPause` (en sortant) |
| **active** | `RESUMED` | `onResume` |

> [!tip] Il manque `onRestart`
> Quand on **revient** sur une activity arrêtée (purgatoire → visible), Android appelle `onRestart` **puis** `onStart`.

---

## 🎯 Quoi faire à quelle étape

| Étape | On y fait | Exemple |
|---|---|---|
| `Application.onCreate` | Ce qui doit exister **pour toute l'app**, une seule fois | **Démarrer Koin** (vrai code de Metrix, ci-dessous) |
| `onCreate` | **Construire** l'écran | `setContent { MetrixApp() }` |
| `onStart` / `onStop` | Ce qui doit tourner **tant que l'écran est visible** | Mises à jour GPS |
| `onResume` / `onPause` | Ce qui doit tourner **seulement au premier plan** | **Capteurs**, caméra, animations |
| `onDestroy` | **Libérer** ce qui reste | Fermer un lecteur, une connexion |

> [!info] La règle des paires
> Ce qu'on **allume** dans une étape, on l'**éteint** dans l'étape **symétrique** : `onStart` ↔ `onStop`, `onResume` ↔ `onPause`. Sinon le capteur continue de tourner app fermée et **vide la batterie**.

### Exemple réel : Metrix démarre Koin dans `Application.onCreate`

```kotlin
class MetrixApplication : Application() {
    override fun onCreate(){
        super.onCreate()

        startKoin {
            //pouvoir logger  et afficher correctement les erreurs
            androidLogger(level = Level.ERROR)

            // Inject Android context
            androidContext(this@MetrixApplication)

            // Load your modules
            modules(appModule)
        }
    }
}
```

| Code | Pourquoi |
|---|---|
| `Application.onCreate` | Appelé **une seule fois**, **avant** toute activity : le bon moment pour préparer Koin |
| `androidContext(this@MetrixApplication)` | Donne à Koin l'**Application Context** → [[11 Le Context]] |

---

## 🧩 Version Activity

```kotlin
class MainActivity : ComponentActivity() {

    private val gyroscopeManager: GyroscopeManager by inject()

    override fun onResume() {
        super.onResume()
        // Cycle de vie : le gyroscope n'est allumé que quand l'activity est au
        // premier plan (onResume). Un capteur allumé en arrière-plan vide la batterie.
        gyroscopeManager.start()
    }

    override fun onPause() {
        // Cycle de vie : on coupe le capteur dès que l'activity quitte le premier plan.
        gyroscopeManager.stop()
        super.onPause()
    }
}
```

| Code | Pourquoi |
|---|---|
| `by inject()` | Koin fournit le Manager → [[12 Les capacités natives (Managers)]] |
| `super.onResume()` **en premier** | On laisse Android finir son travail **avant** d'allumer le capteur |
| `super.onPause()` **en dernier** | On éteint le capteur **avant** qu'Android ne mette l'activity en pause |
| Le commentaire `// Cycle de vie : …` | C'est le **commentaire spécifique** demandé par la grille : il dit **pourquoi** cette étape |

---

## 🎨 Version Compose

En Single Activity, l'écran est un **composable**, pas une activity. Compose a ses propres outils :

```kotlin
@Composable
fun CompassScreen(viewModel: CompassViewModel = koinViewModel()) {
    // Cycle de vie : la boussole n'écoute le gyroscope que quand l'écran est au
    // premier plan (ON_RESUME), et s'arrête dès qu'il passe en arrière-plan
    // (ON_PAUSE) ou qu'on quitte l'écran : on économise la batterie.
    LifecycleResumeEffect(Unit) {
        viewModel.startCompass()
        onPauseOrDispose { viewModel.stopCompass() }
    }

    val state by viewModel.uiState.collectAsStateWithLifecycle()
    CompassContent(state = state)
}
```

| Code | Pourquoi |
|---|---|
| `LifecycleResumeEffect(Unit) { … }` | Le bloc s'exécute à chaque **`onResume`** de l'activity |
| `onPauseOrDispose { … }` | S'exécute à **`onPause`**, ou quand l'écran **disparaît** (navigation) |
| `viewModel.startCompass()` | L'écran **ne touche pas** au capteur : il prévient le ViewModel, qui parle au Manager |
| `collectAsStateWithLifecycle()` | Lit l'état du ViewModel **seulement** quand l'écran est visible → [[03 La couche UI (MVI et UDF)]] |

> [!warning] Dépendance à ajouter
> `LifecycleResumeEffect` et `collectAsStateWithLifecycle` viennent de **`androidx.lifecycle:lifecycle-runtime-compose`** → [[02 Ajouter une dépendance]].

Les autres « effets » Compose :

| Outil | Se déclenche… | Exemple |
|---|---|---|
| `LaunchedEffect(clé)` | quand l'écran **apparaît**, et à chaque changement de **clé** | Metrix recharge la liste au retour : `LaunchedEffect(refreshAfterReturn) { viewModel.loadHosts() }` |
| `DisposableEffect(clé)` | à l'apparition, avec un `onDispose { }` à la **disparition** | S'abonner / se désabonner à quelque chose |
| `LifecycleResumeEffect(clé)` | à **`onResume`** / **`onPause`** de l'activity | Capteurs, caméra |

---

## 🔄 La rotation et le mode nuit

Une rotation ou un passage en mode nuit **recrée** l'activity ([[03 Les Ressources#Reconfiguration cycle de vie complet]]). Qu'est-ce qui survit ?

| Où est la donnée | Survit à la recréation ? |
|---|---|
| Variable dans l'activity | ❌ perdue |
| `remember { }` dans un composable | ❌ perdue |
| `rememberSaveable { }` | ✅ sauvegardée puis restaurée |
| **ViewModel** | ✅ **le ViewModel survit** : c'est une de ses raisons d'exister |

> [!tip] Exemple réel
> Dans Metrix, `HostListViewModel` charge les hôtes dans son `init { loadHosts() }`. Comme le ViewModel survit à la rotation, **on ne recharge pas** la liste en tournant le téléphone.

---

## 🚫 « Logger pour logger »

> [!failure] Ce qui fait perdre des points
> La grille cite comme exemple d'usage **sans objectif pertinent** : mettre un `Log.d` dans chaque callback juste pour montrer qu'on connaît le cycle de vie. Ça ne fait **rien d'utile**.

> [!success] Ce qui en fait gagner
> Une étape du cycle de vie qui sert **à quelque chose** (allumer / éteindre un capteur, démarrer Koin, recharger une liste) **et** un commentaire qui dit **pourquoi cette étape-là**.

---

## 🔗 Liens

- [[02 Les Activities]] — la théorie : les états et les transitions
- [[12 Les capacités natives (Managers)]] — le Manager qu'on allume et éteint
- [[06 KDoc et commentaires]] — comment écrire le commentaire
- [[03 La couche UI (MVI et UDF)]] — l'état qui survit dans le ViewModel
