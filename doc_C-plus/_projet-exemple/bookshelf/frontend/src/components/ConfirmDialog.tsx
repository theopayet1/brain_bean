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
