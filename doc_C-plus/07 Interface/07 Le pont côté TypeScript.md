---
tags:
  - projet/cpp
  - type/guide
  - techno/typescript
  - techno/webview2
  - sujet/ui
  - statut/a-jour
aliases:
  - Bridge TypeScript
  - client.ts
cree: 2026-10-04
maj: 2026-10-04
---

# Le pont côté TypeScript

> [!abstract] En une phrase
> `client.ts` transforme le va-et-vient de messages en **promesses** : `call('addBook', ...)` envoie le message, garde la promesse sous son **id**, et la résout quand le C++ répond. `api.ts` offre **une fonction typée par fonction du pont**, et `types.ts` décrit les données. Les écrans n'utilisent **que** `api.ts`.

---

## 📄 `src/bridge/client.ts`

```ts
// Le transport des appels vers le cœur C++.
//
// Le JavaScript n'est qu'une couche d'AFFICHAGE : toute la logique vit en C++.
// Chaque appel envoie { id, function, request } et reçoit plus tard
// { id, ok: true, data } ou { id, ok: false, code, message }.

type Response =
  | { id: number; ok: true; data: unknown }
  | { id: number; ok: false; code: string; message: string };

interface WebViewHost {
  postMessage(message: string): void;
}

declare global {
  interface Window {
    // Fourni par WebView2 quand la page tourne dans l'application.
    chrome?: { webview?: WebViewHost };
    // Appelé par le C++ pour rendre une réponse.
    __bookshelf_response?: (response: Response) => void;
  }
}

/** Erreur renvoyée par le C++. `message` est déjà une phrase lisible. */
export class BridgeError extends Error {
  readonly code: string;

  constructor(code: string, message: string) {
    super(message);
    this.name = 'BridgeError';
    this.code = code;
  }
}

/** Ce qui sait répondre à un appel : la vraie webview, ou la simulation en développement. */
export type Transport = (fn: string, request: object) => Promise<unknown>;

/** Préfixe qui distingue nos messages de ceux que saucer s'envoie à lui-même. */
const PREFIX = 'bookshelf:';

interface Pending {
  resolve: (data: unknown) => void;
  reject: (error: BridgeError) => void;
}

function createWebViewTransport(host: WebViewHost): Transport {
  // Chaque appel attend SA réponse : on les retrouve grâce à l'id.
  const pending = new Map<number, Pending>();
  let nextId = 1;

  window.__bookshelf_response = (response) => {
    const waiting = pending.get(response.id);
    if (!waiting) {
      return;
    }
    pending.delete(response.id);
    if (response.ok) {
      waiting.resolve(response.data);
    } else {
      waiting.reject(new BridgeError(response.code, response.message));
    }
  };

  return (fn, request) =>
    new Promise((resolve, reject) => {
      const id = nextId++;
      pending.set(id, { resolve, reject });
      host.postMessage(PREFIX + JSON.stringify({ id, function: fn, request }));
    });
}

let transport: Promise<Transport> | undefined;

function getTransport(): Promise<Transport> {
  if (transport) {
    return transport;
  }
  const host = window.chrome?.webview;
  if (host) {
    transport = Promise.resolve(createWebViewTransport(host));
  } else if (import.meta.env.DEV) {
    // Ouvert dans un navigateur normal par `npm run dev` : pas de C++, on simule.
    // Ce module n'est PAS inclus dans l'application livrée.
    transport = import('./simulation').then((module) => module.simulatedTransport);
  } else {
    transport = Promise.reject(
      new BridgeError('bridge.unavailable', "L'application doit être lancée depuis son exe."),
    );
  }
  return transport;
}

/** Appelle une fonction exposée par le C++. Rejette avec une `BridgeError`. */
export async function call<T>(fn: string, request: object = {}): Promise<T> {
  const send = await getTransport();
  return (await send(fn, request)) as T;
}

/** Le message à montrer pour n'importe quelle erreur, sans jamais de détail technique. */
export function errorMessage(error: unknown): string {
  if (error instanceof BridgeError) {
    return error.message;
  }
  return "Une erreur inattendue s'est produite. Réessayez.";
}
```

### Lecture guidée

| Partie | Rôle |
|---|---|
| `window.chrome.webview.postMessage` | Fourni par **WebView2** : envoie un texte au C++ |
| `window.__bookshelf_response` | La fonction que le C++ appelle avec la réponse |
| `pending: Map<number, Pending>` | Les promesses en attente, rangées par **id** |
| `new Promise((resolve, reject) => ...)` | L'appel attend sa réponse sans bloquer l'interface |
| `BridgeError` | Une erreur qui porte le `code` (pour réagir) et le `message` (pour afficher) |
| `import('./simulation')` seulement si `import.meta.env.DEV` | Hors de l'appli (`npm run dev`), un faux C++ répond. **Absent** du build final |
| `errorMessage(e)` | Toujours une phrase affichable, même pour une erreur JS imprévue |

> [!info] Définition — promesse
> Une **promesse** (`Promise`) représente un résultat **qui arrivera plus tard**. On écrit `api.listBooks('').then(succès, échec)` ou `const books = await api.listBooks('')` dans une fonction `async`.

---

## 📄 `src/bridge/api.ts`

```ts
// Une fonction par cas d'usage exposé par le C++ (src/bridge/bridge.cpp).
// Les écrans n'appellent QUE ces fonctions, jamais `call` directement.

import { call } from './client';
import type { Book, NewBook } from './types';

export const listBooks = (search: string) => call<Book[]>('listBooks', { search });

export const addBook = (book: NewBook) => call<{ id: number }>('addBook', book);

export const markAsRead = (id: number, read: boolean) => call<object>('markAsRead', { id, read });

export const removeBook = (id: number) => call<object>('removeBook', { id });
```

## 📄 `src/bridge/types.ts`

```ts
// Les formes des données échangées avec le C++. Miroir de src/bridge/dto.hpp :
// changer un nom ici sans le changer là-bas casse le contrat.

export interface Book {
  id: number;
  title: string;
  author: string;
  /** Absent quand l'année est inconnue. */
  year?: number;
  read: boolean;
  /** « AAAA-MM-JJ ». */
  addedOn: string;
}

export interface NewBook {
  title: string;
  author: string;
  year?: number;
}
```

| Règle | Pourquoi |
|---|---|
| Les écrans appellent `api.addBook(...)`, jamais `call('addBook', ...)` | Le nom de la fonction et les types sont écrits **une fois** |
| `types.ts` est le **miroir** de `dto.hpp` | Mêmes noms de champs. `std::optional<T>` → `champ?: T` |
| Les dates sont des `string` ISO | Comme en C++ ; on les formate à l'affichage |

---

## 🧪 Utiliser l'API dans un écran

```tsx
api.addBook({ title, author, year: year === '' ? undefined : Number(year) }).then(
  ({ id }) => onAdded(id),                 // succès
  (e: unknown) => setError(errorMessage(e)),  // échec : la phrase du C++
);
```

> [!warning] Toujours gérer l'échec
> Une promesse sans gestionnaire d'erreur (`api.addBook(...)` tout seul) avale l'erreur du C++ : l'utilisateur clique et rien ne se passe. La règle ESLint `no-floating-promises` l'interdit.

### Réagir à un code précis

```ts
import { BridgeError } from '../bridge/client';

if (e instanceof BridgeError && e.code === 'book.not_found') {
  onBack();   // le livre a été supprimé ailleurs : retour à la liste
}
```

---

## 🔗 Liens

- [[10 Écrans et composants Preact]] — les écrans qui utilisent l'API
- [[12 Travailler l'interface sans le C++]] — la simulation
- [[08 Le fil de travail]] — la suite
