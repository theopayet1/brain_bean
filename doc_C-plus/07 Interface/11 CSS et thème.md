---
tags:
  - projet/cpp
  - type/guide
  - techno/css
  - sujet/ui
  - statut/a-jour
aliases:
  - Thème CSS
  - Variables CSS
cree: 2026-10-04
maj: 2026-10-04
---

# CSS et thème

> [!abstract] En une phrase
> Toutes les couleurs, polices et tailles sont des **variables CSS** déclarées **une seule fois** dans `:root` (`base.css`) ; le reste du CSS n'utilise que ces variables. Changer le thème = changer quelques lignes. La mise en page utilise **grid** et **flex**, et chaque domaine d'écran a son fichier CSS.

---

## 🎨 Le thème : `styles/base.css`

```css
/* ============================================================
   THÈME — la seule source de vérité des couleurs et tailles.
   Changer une valeur ici change toute l'application.
   ============================================================ */
:root {
  color-scheme: light;

  --accent: #3b5bdb;
  --accent-dark: #2f4ac0;
  --danger: #c92a2a;

  --bg: #f8f9fa;
  --surface: #ffffff;
  --text: #212529;
  --text-muted: #6c757d;
  --border: #dee2e6;

  --font: 'Segoe UI', system-ui, sans-serif;
  --radius: 6px;
  --rail-width: 200px;
}

* {
  box-sizing: border-box;
}

body {
  margin: 0;
  font-family: var(--font);
  background: var(--bg);
  color: var(--text);
}

.btn {
  padding: 6px 14px;
  border: 1px solid var(--border);
  border-radius: var(--radius);
  background: var(--surface);
  cursor: pointer;
}
.btn:disabled {
  opacity: 0.6;
  cursor: default;
}
.btn-primary {
  background: var(--accent);
  border-color: var(--accent);
  color: white;
}
.btn-primary:hover:not(:disabled) {
  background: var(--accent-dark);
}
.btn-danger {
  color: var(--danger);
}

.error {
  color: var(--danger);
}
.empty {
  color: var(--text-muted);
}
.actions {
  display: flex;
  gap: 8px;
  justify-content: flex-end;
}
```

| Point | Pourquoi |
|---|---|
| `--accent`, `--bg`, `--text`… | **Une seule source de vérité** : jamais de `#3b5bdb` écrit ailleurs |
| `color-scheme: light` | Les contrôles natifs (cases à cocher, barres de défilement) suivent |
| `box-sizing: border-box` | La largeur d'un élément **inclut** sa bordure et son padding : plus de calculs faux |
| Classes génériques `.btn`, `.btn-primary`, `.error` | Réutilisées par tous les écrans |

---

## 📐 La mise en page : `styles/books.css`

```css
/* La coquille : un rail de navigation à gauche, le contenu à droite. */
.shell {
  display: grid;
  grid-template-columns: var(--rail-width) 1fr;
  min-height: 100vh;
}
.rail {
  display: flex;
  flex-direction: column;
  gap: 4px;
  padding: 16px;
  background: var(--surface);
  border-right: 1px solid var(--border);
}
.brand {
  font-size: 1.2rem;
  margin: 0 0 16px;
}
.rail-item {
  text-align: left;
  padding: 8px 12px;
  border: none;
  border-radius: var(--radius);
  background: none;
  cursor: pointer;
}
.rail-item.active {
  background: var(--accent);
  color: white;
}
.content {
  padding: 24px 32px;
}

.screen-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
}
.search {
  width: 280px;
  padding: 6px 10px;
  border: 1px solid var(--border);
  border-radius: var(--radius);
}

.table {
  width: 100%;
  border-collapse: collapse;
  background: var(--surface);
}
.table th,
.table td {
  padding: 8px 12px;
  border-bottom: 1px solid var(--border);
  text-align: left;
}

.form {
  display: flex;
  flex-direction: column;
  gap: 12px;
  max-width: 420px;
}
.form label {
  display: flex;
  flex-direction: column;
  gap: 4px;
}
.form input {
  padding: 6px 10px;
  border: 1px solid var(--border);
  border-radius: var(--radius);
}

.overlay {
  position: fixed;
  inset: 0;
  display: grid;
  place-items: center;
  background: rgb(0 0 0 / 0.35);
}
.dialog {
  min-width: 360px;
  padding: 20px 24px;
  border-radius: var(--radius);
  background: var(--surface);
}
```

| Technique | Où | Pour |
|---|---|---|
| `display: grid; grid-template-columns: var(--rail-width) 1fr` | `.shell` | Menu fixe à gauche, contenu qui prend le reste |
| `display: flex; flex-direction: column; gap` | `.rail`, `.form` | Empiler avec un espacement régulier |
| `justify-content: space-between` | `.screen-header` | Titre à gauche, recherche à droite |
| `position: fixed; inset: 0; place-items: center` | `.overlay` | Voile plein écran, boîte centrée |

---

## 🌗 Variantes de thème

Un attribut sur `<html>` + des variables redéfinies :

```css
:root[data-density='comfortable'] {
  --row-padding: 14px;
}
```

```ts
document.documentElement.dataset.density = 'comfortable';
```

Même principe pour un mode sombre (`[data-theme='dark']`).

---

## 🔤 Polices

La CSP interdit les polices en ligne (Google Fonts). Pour une police personnalisée :

1. Mettre les `.woff2` dans `frontend/src/fonts/` (avec leur licence).
2. Les déclarer :
   ```css
   @font-face {
     font-family: 'Inter';
     src: url('../fonts/inter-400.woff2') format('woff2');
     font-weight: 400;
     font-display: block;
   }
   ```
3. Vite les copie dans `dist/assets/`, CMake les embarque. Le type MIME `.woff2` est déjà dans `serveFrontend`.

---

## 🖨️ Impression

Une feuille `@media print` masque l'interface et ne garde que la zone à imprimer :

```css
@media print {
  .rail,
  .screen-header {
    display: none;
  }
  @page {
    size: A4;
    margin: 15mm;
  }
}
```

---

## 🔗 Liens

- [[10 Écrans et composants Preact]] — les classes utilisées
- [[12 Travailler l'interface sans le C++]] — la suite
