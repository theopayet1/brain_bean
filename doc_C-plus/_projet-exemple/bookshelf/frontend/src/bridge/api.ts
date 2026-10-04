// Une fonction par cas d'usage exposé par le C++ (src/bridge/bridge.cpp).
// Les écrans n'appellent QUE ces fonctions, jamais `call` directement.

import { call } from './client';
import type { Book, NewBook } from './types';

export const listBooks = (search: string) => call<Book[]>('listBooks', { search });

export const addBook = (book: NewBook) => call<{ id: number }>('addBook', book);

export const markAsRead = (id: number, read: boolean) => call<object>('markAsRead', { id, read });

export const removeBook = (id: number) => call<object>('removeBook', { id });
