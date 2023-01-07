import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'
import viteCompression from 'vite-plugin-compression'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [preact(), viteCompression({
    deleteOriginFile: true,
    ext: '.gz'
  })],
  build: {
    rollupOptions: {
      output: {
        assetFileNames: (assetInfo) => {
          console.log(assetInfo);
          let extType = assetInfo.name!!.split('.')[1];
          return `web/assets/[name]-[hash][extname]`;
        },
        chunkFileNames: 'web/assets/[name]-[hash].js',
        entryFileNames: 'web/assets/[name]-[hash].js',
      },
    },
  }
});
