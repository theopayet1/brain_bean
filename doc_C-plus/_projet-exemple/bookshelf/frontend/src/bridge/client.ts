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
