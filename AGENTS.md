# Consignes pour les IA

Ce fichier dit à une IA (assistant de code, agent) comment se comporter et comment écrire de la doc dans brain_bean et dans les vaults Obsidian de utilisateur. Les conventions complètes pour les humains sont dans `guide/04-ecrire-une-note.md`.

---

## Comment parler a utilisateur

- **Toujours en français.**
- Il apprend en faisant. **Explique d'abord simplement, puis donne le vrai terme technique en gras** pour qu'il le retienne. Exemple : *« l'app redessine l'écran quand la donnée change, c'est la **recomposition** »*.
- **Guide et pose des questions** plutôt que de décider à sa place. Si le contenu ou la portée n'est pas clair, demande avant d'écrire.
- Réponses directes, sans remplissage. Donne une recommandation plutôt qu'une liste d'options.

---

## Règles strictes

1. **Ne jamais lancer `git commit` ni `git push`.** Prépare les fichiers et donne les commandes. C'est utilisateur qui les lance, sauf s'il donne explicitement l'autorisation, et seulement pour cette fois-là.
2. **Ne jamais t'ajouter comme auteur ou co-auteur** : pas de `Co-Authored-By`, pas de signature d'IA dans les commits, les PR ou les notes.
3. **Aucun fichier `CLAUDE.md`, aucune mention de Claude** dans ce repo ni dans les vaults. Pour des consignes d'IA, on utilise `AGENTS.md`.
4. **Ne jamais modifier le contenu d'un dossier de vault dans brain_bean.** Il est écrasé à chaque synchro. On modifie la note dans son repo source.
5. **Ne pas choisir à la place de utilisateur ** ce qui entre dans `repos.txt`, ni quels vaults synchroniser.
6. **Ne rien produire qui n'a pas été demandé** : ni fichier, ni note, ni refonte.

---

## Structure du repo

| Chemin                             | Rôle                          | Peut-on modifier ?              |
| ---------------------------------- | ----------------------------- | ------------------------------- |
| `README.md`, `AGENTS.md`, `guide/` | Doc du hub                    | Oui, si utilisateur le demande  |
| `repos.txt`                        | Liste des vaults synchronisés | Seulement sur demande explicite |
| `.github/workflows/sync.yml`       | La synchro (GitHub Action)    | Oui, si utilisateur le demande  |
| `<dossier de vault>/`              | Copie synchronisée            | **Non**                         |

---

## Écrire une note

### Front-matter obligatoire

```yaml
---
tags:
  - projet/<nom>        # obligatoire : nom du dossier dans brain_bean
  - type/<type>         # obligatoire : contexte, cdc, archi, decision, setup, guide, reference, astuce, ecran, index
  - techno/<techno>     # une par techno abordée
  - statut/<statut>     # brouillon, a-jour, obsolete
aliases: []
cree: AAAA-MM-JJ
maj: AAAA-MM-JJ
---
```

- Tags en minuscules, sans accents, avec des tirets, en `famille/valeur`.
- **Réutilise les tags existants** avant d'en créer un (cherche dans le vault). Tu peux créer un nouveau `techno/` ou `sujet/`. Une **nouvelle famille** de tags se demande à utilisateur.
- `sujet/<theme>` sert aux thèmes qui traversent plusieurs projets (`sujet/auth`, `sujet/navigation`) : c'est ce qui relie les vaults entre eux.

### Corps de la note

- Un titre `#`, puis 1 ou 2 phrases d'intro : à quoi sert la note, et quoi lire avant.
- **`---` entre chaque grande section.**
- Tableaux pour les décisions et les comparaisons. `> ⚠️ **…**` pour les pièges et les décisions clés.
- Le **pourquoi** des décisions, avec des **dates absolues**.
- Blocs de code avec le langage. Mermaid en **une seule colonne** (`flowchart TB`).
- Images dans `_assets/`, intégrées avec `![[image.png]]`.
- Terminer par **« Voir aussi »**, avec 1 à 5 `[[liens]]` expliqués.
- Ajouter la note au `_index.md` du vault.

### Nommage

- Série ordonnée : `NN-titre-en-kebab.md`. Sous-série : `N.N Titre.md`.
- **Évite les noms déjà utilisés dans d'autres vaults** (collision dans brain_bean). Préfixe avec le projet ou ajoute un `aliases`.

### Commentaires de code (KDoc / `///`)

- Une phrase simple qui dit ce que c'est et à quoi ça sert, avec des liens `[Symbole]`.
- `@param` / `@property` : le rôle **et** le piège éventuel.
- Les `//` dans le corps expliquent le *pourquoi* d'une ligne non évidente, avec des mots de tous les jours.

---

## Avant de rendre ton travail

- [ ] Tout est en français, et les termes techniques sont expliqués
- [ ] Le front-matter est complet (`projet/`, `type/`, `statut/`, les dates)
- [ ] « Voir aussi » est rempli et la note est ajoutée à `_index.md`
- [ ] Aucun `CLAUDE.md`, aucune mention d'IA, aucun commit
- [ ] Tu as donné à utilisateur les commandes git à lancer
