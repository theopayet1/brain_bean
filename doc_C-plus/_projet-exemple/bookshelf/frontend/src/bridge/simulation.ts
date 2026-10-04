// Un FAUX cœur C++, pour travailler l'interface dans un navigateur avec `npm run dev`.
// Jamais inclus dans l'application livrée (import conditionné à import.meta.env.DEV).

import { BridgeError, type Transport } from './client';
import type { Book, NewBook } from './types';

const books: Book[] = [
  { id: 1, title: 'Dune', author: 'Frank Herbert', year: 1965, read: true, addedOn: '2026-09-01' },
  {
    id: 2,
    title: 'Fondation',
    author: 'Isaac Asimov',
    year: 1951,
    read: false,
    addedOn: '2026-09-12',
  },
];
let nextId = 3;

function wait(ms: number) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

export const simulatedTransport: Transport = async (fn, request) => {
  // Un petit délai, comme le vrai C++ : on voit les états « chargement » de l'interface.
  await wait(120);
  switch (fn) {
    case 'listBooks': {
      const { search } = request as { search: string };
      const text = search.toLowerCase();
      return books
        .filter((b) => `${b.title} ${b.author}`.toLowerCase().includes(text))
        .sort((a, b) => a.title.localeCompare(b.title));
    }
    case 'addBook': {
      const book = request as NewBook;
      if (book.title.trim() === '') {
        throw new BridgeError('book.title_missing', 'Le titre est obligatoire.');
      }
      const id = nextId++;
      books.push({ ...book, id, read: false, addedOn: new Date().toISOString().slice(0, 10) });
      return { id };
    }
    case 'markAsRead': {
      const { id, read } = request as { id: number; read: boolean };
      const book = books.find((b) => b.id === id);
      if (!book) {
        throw new BridgeError('book.not_found', "Ce livre n'existe plus.");
      }
      book.read = read;
      return {};
    }
    case 'removeBook': {
      const { id } = request as { id: number };
      const index = books.findIndex((b) => b.id === id);
      if (index >= 0) {
        books.splice(index, 1);
      }
      return {};
    }
    default:
      throw new BridgeError('bridge.unknown_function', `Fonction inconnue : ${fn}`);
  }
};
