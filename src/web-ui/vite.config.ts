import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'
import viteCompression from 'vite-plugin-compression'
import * as path from 'path';

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [preact(), viteCompression({
    deleteOriginFile: true,
    ext: '.gz'
  })],
  resolve: {
    alias: {
      '@': path.resolve(__dirname, 'src'),
    }
  },
  build: {
    rollupOptions: {
      output: {
        assetFileNames: (assetInfo) => {
          console.log(assetInfo);
          return `web/assets/[name]-[hash][extname]`;
        },
        chunkFileNames: 'web/assets/[name]-[hash].js',
        entryFileNames: 'web/assets/[name]-[hash].js',
      },
    },
  }
});
