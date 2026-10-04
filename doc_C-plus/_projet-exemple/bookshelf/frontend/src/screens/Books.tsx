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
