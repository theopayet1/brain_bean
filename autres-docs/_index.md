---
tags:
  - projet/autres-docs
  - type/index
  - statut/a-jour
aliases:
  - Autres docs
cree: 2026-09-28
maj: 2026-09-28
---

# Autres docs — la zone libre de brain_bean

Ici, on écrit **directement dans brain_bean** les notes qui n'ont pas de vault à elles : une astuce, un sujet commun à plusieurs projets, une doc perso, un projet trop petit pour avoir son propre repo.

---

## Ce qui change par rapport aux vaults

| | Dossiers de vaults | `autres-docs/` |
|---|---|---|
| D'où viennent les notes | Copiées depuis leur repo par la synchro | Écrites **ici** |
| Où les modifier | Dans le repo d'origine | **Ici**, dans brain_bean |
| Écrasées à la synchro ? | Oui | **Non, jamais** |

> ⚠️ Le nom `autres-docs` est **réservé** : la synchro refuse de l'utiliser comme dossier de vault, donc rien ne peut écraser cette zone.

---

## Ajouter une note

1. Copier le modèle [[modele-note]] dans ce dossier (ou un sous-dossier par thème).
2. Remplir les propriétés en haut de la note, en particulier le tag `projet/` :
   - la note concerne un projet qui a déjà un vault → **son tag** (`projet/garaxo`…) : elle se rangera avec les notes de ce projet ;
   - sinon → `projet/autres-docs`.
3. Ajouter un tag `sujet/` si la note parle d'un thème commun à plusieurs projets.
4. Ajouter la note dans la liste ci-dessous.
5. Commit + push :

```bash
git add autres-docs
```
```bash
git commit -m "docs: ajoute <nom de la note>"
```
```bash
git push
```

Images : dans `autres-docs/_assets/`, intégrées avec `![[image.png]]`.

---

## Les notes

*(Aucune pour l'instant — ajoute une ligne par note : `- [[nom-de-la-note]] — de quoi elle parle`.)*

---

## Voir aussi
- [[04-ecrire-une-note]] — les conventions (tags, liens, style)
- [[modele-note]] — le modèle à copier
