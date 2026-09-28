---
tags:
  - projet/brain-bean
  - type/guide
  - statut/a-jour
cree: 2026-09-28
maj: 2026-09-28
---

# Démarrer avec brain_bean

Comment récupérer brain_bean sur ton poste et l'ouvrir dans Obsidian. À lire en premier.

---

## 1. Récupérer le repo

Il faut avoir été invité sur le repo : GitHub → **Settings → Collaborators**. Ensuite :

```bash
git clone https://github.com/theopayet1/brain_bean.git
```

Pour récupérer la dernière synchro plus tard :

```bash
git pull
```

---

## 2. L'ouvrir dans Obsidian

1. Ouvrir Obsidian.
2. Cliquer sur **Open folder as vault** (« Ouvrir un dossier comme coffre »).
3. Choisir le dossier `brain_bean`.

Chaque projet apparaît comme un **dossier** à gauche. Toutes leurs notes sont dans le même vault, donc la recherche, le graphe et les tags couvrent **tous les projets à la fois**.

---

## 3. S'y retrouver

- **Panneau Tags** (icône `#` à droite) : un clic sur `#techno/kotlin` ou `#sujet/auth` liste les notes de tous les projets sur ce sujet.
- **Vue graphe** (`Ctrl + G`) : les notes sont des points et les liens des traits. Filtre par tag (`tag:#projet/garaxo`) pour ne voir qu'un projet.
- **`_index.md`** dans chaque dossier de projet : le sommaire du vault, à ouvrir en premier.
- **Recherche** (`Ctrl + Shift + F`) : elle cherche dans tous les vaults d'un coup.

---

## Voir aussi
- [[02-comment-ca-marche]] — ce qui se passe entre ton vault et brain_bean
- [[04-ecrire-une-note]] — les conventions pour que tes notes se relient bien
