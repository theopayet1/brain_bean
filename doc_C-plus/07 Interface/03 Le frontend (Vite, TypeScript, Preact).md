---
tags:
  - projet/cpp
  - type/guide
  - techno/typescript
  - techno/preact
  - techno/vite
  - sujet/ui
  - statut/a-jour
aliases:
  - Frontend Preact
  - Vite TypeScript
cree: 2026-10-04
maj: 2026-10-04
---

# Le frontend (Vite, TypeScript, Preact)

> [!abstract] En une phrase
> Le dossier `frontend/` est un petit projet web indépendant : **Preact** pour les composants, **TypeScript strict** pour les types, **Vite** pour construire, **ESLint** et **Prettier** pour la qualité. `npm run build` produit un dossier `dist/` (un HTML, un JS, un CSS) que CMake embarque dans l'exe.

---

## 📂 Structure

```text
frontend/
├─ package.json          dépendances et scripts
├─ vite.config.ts        la construction
├─ tsconfig.json         TypeScript strict
├─ eslint.config.js      règles de qualité
├─ .prettierrc.json      formatage
├─ index.html            la page unique
└─ src/
   ├─ main.tsx           point d'entrée : affiche <App/>
   ├─ App.tsx            la coquille : menu + écran courant
   ├─ bridge/            tout ce qui parle au C++
   │  ├─ client.ts       le transport (postMessage, promesses)
   │  ├─ api.ts          une fonction par fonction du pont
   │  ├─ types.ts        les formes des données (miroir des DTO C++)
   │  └─ simulation.ts   faux C++ pour npm run dev
   ├─ screens/           un fichier par écran
   ├─ components/        les morceaux réutilisables
   └─ styles/            le CSS
```

---

## 📄 `package.json`

```json
{
  "name": "bookshelf-frontend",
  "private": true,
  "version": "0.1.0",
  "type": "module",
  "scripts": {
    "dev": "vite",
    "build": "tsc --noEmit && vite build",
    "typecheck": "tsc --noEmit",
    "lint": "eslint .",
    "format": "prettier --write .",
    "check": "tsc --noEmit && eslint . && prettier --check . && vite build"
  },
  "dependencies": {
    "preact": "^11.0.0"
  },
  "devDependencies": {
    "@eslint/js": "^10.0.1",
    "eslint": "^10.12.0",
    "prettier": "^3.9.9",
    "typescript": "^6.0.3",
    "typescript-eslint": "^8.71.0",
    "vite": "^8.3.2"
  }
}
```

| Script | Rôle |
|---|---|
| `npm run dev` | Serveur de développement avec rechargement à chaud, dans un navigateur |
| `npm run build` | Vérifie les types **puis** construit `dist/` (c'est ce que CMake appelle) |
| `npm run check` | Tout : types, ESLint, Prettier, build (lancé par `check.ps1`) |
| `npm run format` | Reformate tout |

> [!info] `preact` est la seule dépendance livrée
> Tout le reste est en `devDependencies` : outils de développement, absents du `dist/`.

---

## 📄 `vite.config.ts`

```ts
import { defineConfig } from 'vite';

export default defineConfig({
  // Chemins relatifs : le dist/ est servi depuis l'exe par le schéma app://,
  // pas depuis la racine d'un serveur web.
  base: './',
  build: {
    // WebView2 suit Edge, toujours à jour : inutile de viser de vieux navigateurs.
    target: 'esnext',
    modulePreload: { polyfill: false },
    // Jamais d'image transformée en texte base64 dans le JS : chaque fichier reste un fichier.
    assetsInlineLimit: 0,
    rollupOptions: {
      output: {
        // Noms FIXES, sans empreinte (pas de main-3f9a2c.js) : la liste des fichiers que
        // CMake embarque dans l'exe ne doit pas changer à chaque build.
        entryFileNames: 'assets/[name].js',
        chunkFileNames: 'assets/[name].js',
        assetFileNames: 'assets/[name][extname]',
      },
    },
  },
});
```

| Réglage | Pourquoi |
|---|---|
| `base: './'` | Chemins relatifs : la page est servie par `app://`, pas par la racine d'un site |
| `target: 'esnext'` | WebView2 est toujours à jour |
| `assetsInlineLimit: 0` | Pas d'image en base64 dans le JS (et la CSP reste simple) |
| Noms **sans empreinte** (`[name].js`) | CMake liste les fichiers à embarquer à la configuration : la liste ne doit pas changer à chaque build ([[04 Embarquer le frontend dans l'exe]]) |

---

## 📄 `tsconfig.json`

```json
{
  "compilerOptions": {
    "target": "ES2022",
    "module": "ESNext",
    "moduleResolution": "bundler",
    "lib": ["ES2022", "DOM", "DOM.Iterable"],
    "types": ["vite/client"],
    "jsx": "react-jsx",
    "jsxImportSource": "preact",
    "strict": true,
    "noUncheckedIndexedAccess": true,
    "noImplicitOverride": true,
    "noFallthroughCasesInSwitch": true,
    "noUnusedLocals": true,
    "noUnusedParameters": true,
    "verbatimModuleSyntax": true,
    "isolatedModules": true,
    "skipLibCheck": true,
    "noEmit": true
  },
  "include": ["src", "vite.config.ts"]
}
```

| Option | Pourquoi |
|---|---|
| `strict` | Toutes les vérifications de TypeScript |
| `noUncheckedIndexedAccess` | `liste[3]` est de type `T \| undefined` : oblige à tester |
| `jsx: react-jsx` + `jsxImportSource: preact` | Le JSX est compilé pour Preact |
| `noEmit` | TypeScript **vérifie** seulement ; c'est Vite qui construit |

---

## 📄 `eslint.config.js`

```js
import js from '@eslint/js';
import { defineConfig } from 'eslint/config';
import tseslint from 'typescript-eslint';

export default defineConfig(
  { ignores: ['dist/', 'node_modules/', 'eslint.config.js'] },
  js.configs.recommended,
  tseslint.configs.strictTypeChecked,
  {
    languageOptions: {
      parserOptions: {
        projectService: true,
        tsconfigRootDir: import.meta.dirname,
      },
    },
    rules: {
      // Une promesse oubliée = une erreur du C++ qui ne s'affiche jamais.
      '@typescript-eslint/no-floating-promises': 'error',
      '@typescript-eslint/no-misused-promises': ['error', { checksVoidReturn: false }],
      '@typescript-eslint/restrict-template-expressions': ['error', { allowNumber: true }],
      '@typescript-eslint/no-confusing-void-expression': ['error', { ignoreArrowShorthand: true }],
      // Jamais de HTML injecté : ce que tape l'utilisateur doit rester du texte.
      'no-restricted-syntax': [
        'error',
        {
          selector: "JSXAttribute[name.name='dangerouslySetInnerHTML']",
          message: 'Pas de HTML injecté : le texte saisi doit rester du texte.',
        },
        {
          selector: "MemberExpression[property.name='innerHTML']",
          message: 'Pas de innerHTML : le texte saisi doit rester du texte.',
        },
      ],
    },
  },
);
```

| Règle | Pourquoi |
|---|---|
| `strictTypeChecked` | Les règles les plus strictes, qui utilisent les types |
| `no-floating-promises` | Une promesse sans `.then(..., erreur)` ni `await` = une erreur du C++ **jamais affichée** |
| Interdire `dangerouslySetInnerHTML` et `innerHTML` | Le texte saisi est toujours affiché **comme du texte**, jamais interprété comme du HTML (pas d'injection) |

---

## 📄 `index.html` et `main.tsx`

```html
<!doctype html>
<html lang="fr">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Bookshelf</title>
  </head>
  <body>
    <div id="app"></div>
    <script type="module" src="/src/main.tsx"></script>
  </body>
</html>
```

```tsx
import { render } from 'preact';

import { App } from './App';

import './styles/base.css';
import './styles/books.css';

const root = document.querySelector<HTMLDivElement>('#app');
if (!root) {
  throw new Error('Élément #app introuvable dans index.html');
}

render(<App />, root);
```

`render(<App />, root)` : Preact dessine le composant `App` dans la `<div id="app">`.

---

## 📄 `App.tsx` : la coquille

```tsx
import { useState } from 'preact/hooks';

import { AddBook } from './screens/AddBook';
import { Books } from './screens/Books';

/** Les écrans possibles. Pas de routeur : un simple état suffit pour une app de bureau. */
type Screen = 'books' | 'add';

export function App() {
  const [screen, setScreen] = useState<Screen>('books');

  return (
    <div class="shell">
      <nav class="rail">
        <h1 class="brand">Bookshelf</h1>
        <button
          type="button"
          class={screen === 'books' ? 'rail-item active' : 'rail-item'}
          onClick={() => setScreen('books')}
        >
          Mes livres
        </button>
        <button
          type="button"
          class={screen === 'add' ? 'rail-item active' : 'rail-item'}
          onClick={() => setScreen('add')}
        >
          Ajouter
        </button>
      </nav>
      <main class="content">
        {screen === 'books' && <Books />}
        {screen === 'add' && (
          <AddBook onAdded={() => setScreen('books')} onCancel={() => setScreen('books')} />
        )}
      </main>
    </div>
  );
}
```

> [!tip] Pas de routeur
> Une application de bureau n'a pas d'URL à partager : un simple état `screen` suffit pour choisir l'écran. Pour un écran avec paramètre (« fiche du livre 12 »), un type union : `{ screen: 'book'; id: number }`.

---

## 🔗 Liens

- [[10 Écrans et composants Preact]] — écrire les écrans
- [[04 Embarquer le frontend dans l'exe]] — la suite
