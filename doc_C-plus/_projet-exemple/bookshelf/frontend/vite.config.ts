import { defineConfig } from 'vite';

export default defineConfig({
  // Chemins relatifs : le dist/ est servi depuis l'exe par le schéma app://,
  // pas depuis la racine d'un serveur web.
  base: './',
  build: {
    // WebView2 suit Edge, toujours à jour : inutile de viser de vieux navigateurs.
    target: 'esnext',
    modulePreload: { polyfill: false },
    // Jamais d'image transformée en texte base64 dans le JS : chaque fichier reste un fichier.
    assetsInlineLimit: 0,
    rollupOptions: {
      output: {
        // Noms FIXES, sans empreinte (pas de main-3f9a2c.js) : la liste des fichiers que
        // CMake embarque dans l'exe ne doit pas changer à chaque build.
        entryFileNames: 'assets/[name].js',
        chunkFileNames: 'assets/[name].js',
        assetFileNames: 'assets/[name][extname]',
      },
    },
  },
});
