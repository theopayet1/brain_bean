# 🧠 brain_bean

Mon second cerveau : tous mes vaults Obsidian, un par projet, réunis automatiquement au même endroit.

> ⚠️ **Les dossiers des vaults sont en lecture seule** : ils sont écrasés à chaque synchro. Pour modifier une de leurs notes, il faut passer par son repo d'origine. Pour écrire directement ici, il y a `autres-docs/`.

---

## Par où commencer

| Tu veux… | Lis |
|---|---|
| Ouvrir brain_bean dans Obsidian et t'y retrouver | [guide/01-demarrer.md](guide/01-demarrer.md) |
| Comprendre comment la synchro fonctionne | [guide/02-comment-ca-marche.md](guide/02-comment-ca-marche.md) |
| Ajouter ou retirer un vault | [guide/03-ajouter-un-vault.md](guide/03-ajouter-un-vault.md) |
| Écrire une note qui se relie bien aux autres (tags, liens) | [guide/04-ecrire-une-note.md](guide/04-ecrire-une-note.md) |
| Ajouter une doc directement ici, sans vault | [autres-docs/_index.md](autres-docs/_index.md) |
| Faire écrire de la doc par une IA | [AGENTS.md](AGENTS.md) |

---

## Structure du repo

```
brain_bean/
├── README.md                  # Ce fichier
├── AGENTS.md                  # Consignes pour les IA qui écrivent de la doc
├── repos.txt                  # La liste des vaults à synchroniser
├── guide/                     # Mode d'emploi pour les humains
│   ├── 01-demarrer.md
│   ├── 02-comment-ca-marche.md
│   ├── 03-ajouter-un-vault.md
│   ├── 04-ecrire-une-note.md
│   └── modele-note.md         # Modèle de note à copier
├── autres-docs/               # Zone libre : notes écrites directement ici
│   └── _index.md
├── .github/workflows/sync.yml # La synchro automatique
└── <un dossier par vault>/    # Créés par la synchro, ne pas modifier à la main
```

---

## En bref

- Toutes les **6 h**, une GitHub Action copie chaque vault listé dans `repos.txt` dans son propre dossier.
- Elle lit les repos privés grâce au secret `DOCS_READ_TOKEN`, un token GitHub en **lecture seule**.
- Le dossier `autres-docs/` est la **zone libre** : on y écrit directement, et la synchro n'y touche jamais.
- Pendant la copie, les dossiers `.git/`, `.obsidian/` et `.claude/` et les fichiers `CLAUDE.md` sont retirés.
- brain_bean est **public** : c'est la vitrine de mes docs. Les repos d'origine peuvent rester privés, et seul ce qui est listé dans `repos.txt` est publié ici.
"# dotnet_doc" 
