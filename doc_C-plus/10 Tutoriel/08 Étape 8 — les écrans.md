---
tags:
  - projet/cpp
  - type/guide
  - techno/preact
  - techno/typescript
  - techno/css
  - sujet/ui
  - statut/a-jour
aliases:
  - Tutoriel étape 8
cree: 2026-10-04
maj: 2026-10-04
---

# Étape 8 — les écrans

> [!abstract] En une phrase
> On écrit le côté TypeScript du pont (`client.ts`, `api.ts`, `types.ts`, `simulation.ts`), puis les écrans (liste, ajout, confirmation) et le CSS. On les développe dans le navigateur avec la simulation, puis on vérifie dans la vraie appli. À lire avant : [[07 Le pont côté TypeScript]] et [[10 Écrans et composants Preact]].

---

## 📄 1. Le pont TypeScript

Dans `frontend/src/bridge/` : `types.ts`, `client.ts`, `api.ts`, `simulation.ts`.

## 📄 2. Les écrans

| Fichier | Rôle |
|---|---|
| `src/App.tsx` | La coquille : menu à gauche, écran courant à droite |
| `src/screens/Books.tsx` | Liste + recherche + case « lu » + suppression |
| `src/screens/AddBook.tsx` | Formulaire d'ajout |
| `src/components/ConfirmDialog.tsx` | Confirmation d'une suppression |
| `src/styles/base.css`, `books.css` | Thème et mise en page ([[11 CSS et thème]]) |
| `src/main.tsx` | Importe les CSS et affiche `<App />` |

## 🔁 3. Développer

```powershell
cd frontend
npm run dev          # dans le navigateur, avec la simulation
npm run check        # types, lint, format, build
```

Puis dans la vraie appli :

```powershell
cmake --build --preset debug
.\build\debug\src\app\Bookshelf.exe
```

Ajouter un livre, chercher, cocher « lu », supprimer, fermer l'appli, la rouvrir : **les livres sont toujours là**.

---

## ✅ Point d'étape

- [ ] `npm run check` vert
- [ ] Toutes les actions marchent dans la vraie appli
- [ ] Un titre vide affiche « Le titre est obligatoire. » (le message vient du C++)
- [ ] Commit : « Écrans : liste et ajout »

---

## 🔗 Liens

- [[12 Travailler l'interface sans le C++]] — la simulation
- [[09 Étape 9 — livrer]] — la suite
