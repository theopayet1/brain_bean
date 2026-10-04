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
