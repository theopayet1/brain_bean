---
tags:
  - projet/brain-bean
  - type/guide
  - techno/github-actions
  - statut/a-jour
cree: 2026-09-28
maj: 2026-09-28
---

# Ajouter (ou retirer) un vault

Trois étapes : donner l'accès au token, ajouter une ligne à `repos.txt`, pusher.

---

## 1. Donner l'accès au token

Le token ne lit que les repos que tu lui as explicitement donnés.

1. GitHub → avatar → **Settings → Developer settings → Personal access tokens → Fine-grained tokens**.
2. Ouvrir le token de brain_bean, puis **Edit**.
3. Dans **Repository access**, cocher le nouveau repo.
4. **Update**.

---

## 2. Ajouter une ligne à `repos.txt`

Format : jusqu'à trois colonnes séparées par des espaces. **Seule la 1re est obligatoire.**

```
<repo>  <dossier à copier>  <nom du dossier dans brain_bean>
```

| Colonne | Quoi mettre | Si tu la laisses vide |
|---|---|---|
| Repo | L'**URL** GitHub (`https://github.com/theopayet1/docsbox.git`), `theopayet1/docsbox`, ou juste `docsbox` | — (obligatoire) |
| Dossier à copier | `.` pour tout le repo, sinon le sous-dossier (ex. `docs`) | `.` : tout le repo |
| Dossier dans brain_bean | Nom court, en minuscules. **Il devient aussi le tag `projet/…`** | le nom du repo |

Exemples :

```
https://github.com/theopayet1/doc_kotlin.git
docsbox             .     sandbox
https://github.com/theopayet1/sass_garage.git   docs  garaxo
```

- La 1re ligne copie **tout** `doc_kotlin` dans le dossier `doc_kotlin/`.
- La 2e copie tout `docsbox` dans `sandbox/`.
- La 3e copie **seulement** `docs/` de `sass_garage` dans `garaxo/`.

> ⚠️ **brain_bean est public : ajouter une ligne, c'est publier.** Tout ce qui est copié devient lisible par n'importe qui, même si le repo d'origine est privé. Avant d'ajouter un repo, vérifie ce qu'il contient, et utilise la 2e colonne (ex. `docs`) pour ne publier qu'une partie.

> ⚠️ **Si le vault est dans un repo de code**, mets le sous-dossier de la doc (`docs`) et pas `.`, sinon tout le code est copié.

> ⚠️ **Noms réservés :** `guide`, `autres-docs` et `.github` ne peuvent pas servir de dossier de destination. La synchro s'arrête en erreur si tu les utilises, pour ne pas les écraser.

---

## 3. Pusher

```bash
git add repos.txt
```
```bash
git commit -m "chore: ajoute le vault mon-app"
```
```bash
git push
```

La modification de `repos.txt` relance la synchro toute seule. Vérifie dans **Actions** qu'elle est verte, puis fais `git pull`.

---

## Retirer un vault

1. Supprimer sa ligne dans `repos.txt`.
2. Supprimer son dossier dans brain_bean (`git rm -r nom-du-dossier`).
3. Commit + push.
4. *(Optionnel)* Retirer le repo du token.

---

## Si la synchro est rouge

| Message dans le log | Cause probable |
|---|---|
| `Repository not found` | Le repo n'est pas coché dans le token, ou il y a une faute de frappe dans `repos.txt` |
| `Authentication failed` / `Bad credentials` | Le token a expiré : il faut le régénérer et remplacer le secret |
| `cp: cannot stat 'tmp/docs/.'` | Le dossier à copier (2e colonne) n'existe pas dans ce repo |
| `'…' is a reserved folder` | La 3e colonne de `repos.txt` utilise un nom réservé (`guide`, `autres-docs`…) |
| `Permission denied` au `git push` | **Settings → Actions → General → Workflow permissions** n'est pas sur *Read and write* |

---

## 🔗 Liens
- [[02-comment-ca-marche]] — le détail de la synchro
- [[04-ecrire-une-note]] — préparer les notes du nouveau vault
