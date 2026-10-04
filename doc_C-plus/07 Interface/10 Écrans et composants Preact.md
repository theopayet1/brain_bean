---
tags:
  - projet/cpp
  - type/guide
  - techno/preact
  - techno/typescript
  - sujet/ui
  - statut/a-jour
aliases:
  - Preact hooks
  - Composants Preact
cree: 2026-10-04
maj: 2026-10-04
---

# Écrans et composants Preact

> [!abstract] En une phrase
> Un **composant** Preact est une fonction qui reçoit des **props** et renvoie du **JSX** (du HTML dans le code). Il garde ses données dans des **états** (`useState`) ; quand un état change, Preact **redessine** le composant. Les appels au C++ se font dans des **effets** (`useEffect`) ou des gestionnaires d'événements. C'est exactement l'API de React, en plus léger.

---

## 🧩 Les briques

```tsx
import { useEffect, useState } from 'preact/hooks';

interface CounterProps {
  start: number;            // une prop : donnée par le parent
}

export function Counter({ start }: CounterProps) {
  const [count, setCount] = useState(start);    // un état : appartient au composant

  useEffect(() => {
    document.title = `Compteur : ${count}`;     // un effet : après chaque dessin où count a changé
  }, [count]);

  return (
    <button type="button" class="btn" onClick={() => setCount(count + 1)}>
      {count}
    </button>
  );
}
```

| Notion | Sens |
|---|---|
| **Props** | Les paramètres du composant, donnés par le parent, **en lecture seule** |
| **État** (`useState`) | Une donnée qui appartient au composant ; la changer **redessine** |
| **Effet** (`useEffect(f, [deps])`) | Du code lancé **après** le dessin, quand une dépendance change (`[]` = une seule fois) |
| **Référence** (`useRef`) | Une valeur gardée entre deux dessins **sans** redessiner (un compteur, un élément HTML) |
| **JSX** | `<div class="x">{valeur}</div>` : `{...}` insère une expression. En Preact, `class` (pas `className`) |

> [!info] Définition — hook
> Les fonctions `useXxx` sont des **hooks**. Règle : les appeler **toujours au début** du composant, jamais dans un `if` ou une boucle.

---

## 📄 Un écran complet : la liste des livres

```tsx
// La liste des livres, avec une recherche. Le filtrage est fait par le C++.

import { useEffect, useRef, useState } from 'preact/hooks';

import { ConfirmDialog } from '../components/ConfirmDialog';
import * as api from '../bridge/api';
import { errorMessage } from '../bridge/client';
import type { Book } from '../bridge/types';

export function Books() {
  const [search, setSearch] = useState('');
  const [books, setBooks] = useState<Book[] | null>(null);
  const [error, setError] = useState('');
  const [toDelete, setToDelete] = useState<Book | null>(null);
  const [reload, setReload] = useState(0);

  // Chaque frappe relance la requête. Seule la réponse à la DERNIÈRE demande compte :
  // une réponse plus ancienne arrivée en retard ne doit pas écraser un résultat plus récent.
  const lastRequest = useRef(0);
  useEffect(() => {
    const number = ++lastRequest.current;
    api.listBooks(search).then(
      (list) => {
        if (number === lastRequest.current) {
          setBooks(list);
          setError('');
        }
      },
      (e: unknown) => {
        if (number === lastRequest.current) {
          setError(errorMessage(e));
        }
      },
    );
  }, [search, reload]);

  const toggleRead = (book: Book) => {
    api.markAsRead(book.id, !book.read).then(
      () => setReload((n) => n + 1),
      (e: unknown) => setError(errorMessage(e)),
    );
  };

  const confirmDelete = () => {
    if (!toDelete) {
      return;
    }
    api.removeBook(toDelete.id).then(
      () => {
        setToDelete(null);
        setReload((n) => n + 1);
      },
      (e: unknown) => setError(errorMessage(e)),
    );
  };

  return (
    <section>
      <header class="screen-header">
        <h2>Mes livres</h2>
        <input
          type="search"
          class="search"
          placeholder="Titre ou auteur…"
          value={search}
          onInput={(e) => setSearch(e.currentTarget.value)}
        />
      </header>

      {error && (
        <p class="error" role="alert">
          {error}
        </p>
      )}

      {books === null ? null : books.length === 0 ? (
        <p class="empty">Aucun livre.</p>
      ) : (
        <table class="table">
          <thead>
            <tr>
              <th>Titre</th>
              <th>Auteur</th>
              <th>Année</th>
              <th>Lu</th>
              <th />
            </tr>
          </thead>
          <tbody>
            {books.map((book) => (
              <tr key={book.id}>
                <td>{book.title}</td>
                <td>{book.author}</td>
                <td>{book.year ?? '—'}</td>
                <td>
                  <input
                    type="checkbox"
                    checked={book.read}
                    onChange={() => toggleRead(book)}
                    aria-label={`Marquer « ${book.title} » comme lu`}
                  />
                </td>
                <td>
                  <button type="button" class="btn btn-danger" onClick={() => setToDelete(book)}>
                    Supprimer
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      )}

      {toDelete && (
        <ConfirmDialog
          title="Supprimer ce livre ?"
          confirmLabel="Supprimer"
          onConfirm={confirmDelete}
          onCancel={() => setToDelete(null)}
        >
          « {toDelete.title} » sera définitivement supprimé.
        </ConfirmDialog>
      )}
    </section>
  );
}
```

### Lecture guidée

| Partie | Pourquoi |
|---|---|
| `books: Book[] \| null` | `null` = pas encore chargé (on n'affiche rien), `[]` = chargé mais vide (« Aucun livre ») |
| `useEffect(..., [search, reload])` | Recharge quand la recherche change, ou quand on incrémente `reload` après une modification |
| `lastRequest` (un `useRef`) | Chaque frappe lance une requête. Si une **ancienne** réponse arrive **après** une récente, on l'ignore. Sinon la liste afficherait le résultat d'une recherche déjà dépassée |
| `onInput={(e) => setSearch(e.currentTarget.value)}` | Champ **contrôlé** : la valeur affichée vient de l'état |
| `key={book.id}` | Aide Preact à reconnaître chaque ligne quand la liste change |
| `{error && <p ...>}` | Affichage conditionnel |
| `role="alert"` | Les lecteurs d'écran annoncent le message |
| `toDelete` + `<ConfirmDialog>` | Une action irréversible demande confirmation |

> [!tip] Le filtrage est fait par le C++
> L'écran envoie `search` au pont et affiche ce qui revient. Il ne filtre ni ne trie lui-même : une seule source de vérité, et ça reste rapide avec 10 000 livres (index SQL).

---

## 📄 Un formulaire

```tsx
// Le formulaire d'ajout. La validation qui COMPTE est faite par le C++ : ici on ne fait
// qu'afficher le message qu'il renvoie.

import { useState } from 'preact/hooks';

import * as api from '../bridge/api';
import { errorMessage } from '../bridge/client';

interface AddBookProps {
  onAdded: (id: number) => void;
  onCancel: () => void;
}

export function AddBook({ onAdded, onCancel }: AddBookProps) {
  const [title, setTitle] = useState('');
  const [author, setAuthor] = useState('');
  const [year, setYear] = useState('');
  const [error, setError] = useState('');
  const [saving, setSaving] = useState(false);

  const submit = (event: Event) => {
    event.preventDefault(); // sinon le navigateur recharge la page
    setSaving(true);
    api
      .addBook({ title, author, year: year === '' ? undefined : Number(year) })
      .then(
        ({ id }) => onAdded(id),
        (e: unknown) => setError(errorMessage(e)),
      )
      .finally(() => setSaving(false));
  };

  return (
    <form class="form" onSubmit={submit}>
      <h2>Ajouter un livre</h2>
      <label>
        Titre
        <input value={title} onInput={(e) => setTitle(e.currentTarget.value)} required />
      </label>
      <label>
        Auteur
        <input value={author} onInput={(e) => setAuthor(e.currentTarget.value)} />
      </label>
      <label>
        Année
        <input
          type="number"
          value={year}
          onInput={(e) => setYear(e.currentTarget.value)}
          min={1400}
          max={2100}
        />
      </label>
      {error && (
        <p class="error" role="alert">
          {error}
        </p>
      )}
      <div class="actions">
        <button type="button" class="btn" onClick={onCancel} disabled={saving}>
          Annuler
        </button>
        <button type="submit" class="btn btn-primary" disabled={saving}>
          {saving ? 'Enregistrement…' : 'Ajouter'}
        </button>
      </div>
    </form>
  );
}
```

| Point | Pourquoi |
|---|---|
| `event.preventDefault()` | Sinon le navigateur « envoie » le formulaire et recharge la page |
| `saving` | Désactive les boutons pendant l'appel : pas de double envoi |
| `required`, `min`, `max` | Confort de saisie **seulement**. La vraie validation est en C++ ; son message s'affiche dans `error` |
| `.finally(() => setSaving(false))` | Réactive les boutons dans tous les cas |
| `onAdded`, `onCancel` en props | L'écran ne sait pas où aller ensuite : c'est le parent (`App`) qui décide |

---

## 📄 Un composant réutilisable : la confirmation

```tsx
// Boîte de confirmation d'une action irréversible. Le bouton qui a le focus est « Annuler » :
// un appui trop rapide sur Entrée ne détruit rien.

import type { ComponentChildren } from 'preact';
import { useEffect, useLayoutEffect, useRef } from 'preact/hooks';

interface ConfirmDialogProps {
  title: string;
  confirmLabel: string;
  children: ComponentChildren;
  onConfirm: () => void;
  onCancel: () => void;
}

export function ConfirmDialog({
  title,
  confirmLabel,
  children,
  onConfirm,
  onCancel,
}: ConfirmDialogProps) {
  const cancel = useRef<HTMLButtonElement>(null);

  // Avant même que l'écran soit redessiné : le focus va sur « Annuler ».
  useLayoutEffect(() => {
    cancel.current?.focus();
  }, []);

  // Échap ferme la boîte.
  useEffect(() => {
    const onKey = (e: KeyboardEvent) => {
      if (e.key === 'Escape') {
        onCancel();
      }
    };
    document.addEventListener('keydown', onKey);
    return () => document.removeEventListener('keydown', onKey);
  }, [onCancel]);

  return (
    <div
      class="overlay"
      onClick={(e) => {
        // Un clic sur le voile gris (pas sur la boîte) ferme.
        if (e.target === e.currentTarget) {
          onCancel();
        }
      }}
    >
      <div class="dialog" role="alertdialog" aria-modal="true" aria-labelledby="dialog-title">
        <h3 id="dialog-title">{title}</h3>
        <div>{children}</div>
        <div class="actions">
          <button type="button" class="btn" ref={cancel} onClick={onCancel}>
            Annuler
          </button>
          <button type="button" class="btn btn-danger" onClick={onConfirm}>
            {confirmLabel}
          </button>
        </div>
      </div>
    </div>
  );
}
```

| Point | Pourquoi |
|---|---|
| `children: ComponentChildren` | Le contenu entre les balises `<ConfirmDialog>...</ConfirmDialog>` |
| `useLayoutEffect` + `focus()` | Le focus va sur **Annuler** avant même le dessin : un Entrée trop rapide ne supprime rien |
| `useEffect` qui écoute `keydown` et **se nettoie** (`return () => ...`) | Échap ferme ; l'écouteur est retiré quand la boîte disparaît |
| Clic sur le voile (`e.target === e.currentTarget`) | Fermer en cliquant à côté |
| `role="alertdialog"`, `aria-modal`, `aria-labelledby` | Accessibilité |

---

## 📏 Règles pour les écrans

| Règle | Pourquoi |
|---|---|
| Un fichier par écran dans `screens/`, un par composant réutilisable dans `components/` | On s'y retrouve |
| Les écrans n'appellent que `api.*` | Voir [[07 Le pont côté TypeScript]] |
| Toujours gérer l'échec d'un appel (`errorMessage`) | Pas d'erreur silencieuse |
| Pas de logique métier en JS | Elle est en C++ ; le JS affiche |
| Les props de navigation sont des callbacks (`onAdded`, `onBack`) | L'écran ne connaît pas les autres écrans |
| États de chargement explicites (`null`, `saving`) | Pas d'écran qui clignote, pas de double clic |

---

## 🔗 Liens

- [[11 CSS et thème]] — la mise en forme
- [[12 Travailler l'interface sans le C++]] — développer ces écrans dans un navigateur
