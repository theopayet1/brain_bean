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
