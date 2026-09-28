---
tags:
  - projet/brain-bean
  - type/guide
  - type/reference
  - statut/a-jour
aliases:
  - Conventions de doc
cree: 2026-09-28
maj: 2026-09-28
---

# Écrire une note qui se relie bien

Les conventions à suivre dans **chaque vault**, pour qu'une fois dans brain_bean les notes des différents projets se retrouvent entre elles. Le modèle prêt à copier est dans [[modele-note]].

---

## Les propriétés en haut de la note

Chaque note commence par un bloc de **propriétés**, appelé **front-matter** : du YAML entre deux lignes `---`. Obsidian l'affiche comme un petit formulaire.

```yaml
---
tags:
  - projet/garaxo
  - type/archi
  - techno/csharp
  - statut/a-jour
aliases:
  - Archi backend
cree: 2026-09-28
maj: 2026-09-28
---
```

| Propriété | À quoi elle sert |
|---|---|
| `tags` | Classer la note (voir plus bas). **Obligatoire** |
| `aliases` | D'autres noms pour la note : `[[Archi backend]]` mènera à elle |
| `cree` / `maj` | Date de création et de dernière mise à jour |

---

## Les tags : ce qui relie les projets

Un tag est une **étiquette**. Ils sont **hiérarchiques** : `famille/valeur`. On les écrit en minuscules, sans accents, avec des tirets.

| Famille | Sert à dire… | Exemples |
|---|---|---|
| `projet/` | de quel projet vient la note (**obligatoire**) | `projet/garaxo`, `projet/sandbox` |
| `type/` | quel genre de note c'est (**obligatoire**) | `type/archi`, `type/guide`, `type/decision`, `type/setup`, `type/astuce` |
| `techno/` | les technos abordées | `techno/kotlin`, `techno/csharp`, `techno/scss` |
| `statut/` | si la note est à jour | `statut/brouillon`, `statut/a-jour`, `statut/obsolete` |
| `sujet/` | un thème commun à plusieurs projets | `sujet/auth`, `sujet/navigation`, `sujet/ui` |

**Pourquoi c'est important :** dans brain_bean, cliquer sur `#sujet/auth` montre la note d'auth de garaxo **et** celle d'un autre projet. Sans tags communs, les vaults restent des îles.

> 💡 Avant d'inventer un tag, regarde dans le panneau **Tags** s'il en existe déjà un proche. `techno/csharp` et `techno/c-sharp` feraient deux tags différents.

---

## Les liens

| Pour lier… | Écrire |
|---|---|
| Une note du même vault | `[[06-architecture-backend-csharp]]` |
| Avec un autre texte affiché | `[[06-architecture-backend-csharp\|archi backend]]` |
| Une section précise | `[[06-architecture-backend-csharp#Multi-tenant]]` |
| Une image | `![[mon-image.png]]`, rangée dans `_assets/` |
| Un site | `[texte](https://…)` |

- Un lien vers une note **d'un autre projet** ne marche que dans brain_bean. Préfère un **tag commun**.
- **Termine chaque note par « Voir aussi »**, avec 1 à 5 liens, chacun expliqué.
- **Chaque vault a un `_index.md`**, le sommaire qui liste ses notes. Ajoute-y chaque nouvelle note.

---

## Nommer les fichiers

| Cas | Format | Exemple |
|---|---|---|
| Série à lire dans l'ordre | `NN-titre.md` | `04-architecture-vue-ensemble.md` |
| Sous-parties d'un sujet | `N.N Titre.md` | `1.1 États du panel.md` |
| Sommaire du vault | `_index.md` | — |

> ⚠️ **Évite les noms trop génériques** (`01-cahier-des-charges.md`) : dans brain_bean, plusieurs projets peuvent avoir le même nom de fichier et `[[01-cahier-des-charges]]` devient ambigu. Ajoute le projet dans le nom ou un `aliases`.

---

## Le style

- **En français**, avec des phrases courtes.
- **D'abord l'idée simple, puis le vrai mot technique en gras.**
- **Le pourquoi des décisions**, avec la date : *« décidé le 9 juillet 2026 parce que… »*.
- **Un `---` entre les grandes sections**, des tableaux pour les décisions, `> ⚠️` pour les pièges.
- **Schémas Mermaid en une colonne** (`flowchart TB`) pour qu'ils restent lisibles dans Obsidian.
- **Un sujet par note** : si elle devient trop longue, découpe-la en série numérotée.

---

## Voir aussi
- [[modele-note]] — le modèle à copier
- [[01-demarrer]] — utiliser les tags et le graphe dans brain_bean
