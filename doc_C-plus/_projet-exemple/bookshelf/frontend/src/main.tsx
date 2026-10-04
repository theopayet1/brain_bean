import { render } from 'preact';

import { App } from './App';

import './styles/base.css';
import './styles/books.css';

const root = document.querySelector<HTMLDivElement>('#app');
if (!root) {
  throw new Error('Élément #app introuvable dans index.html');
}

render(<App />, root);
