import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'
import { compression } from 'vite-plugin-compression2'

// https://vitejs.dev/config/
export default defineConfig({
  plugins: [preact(), compression({
    deleteOriginalAssets: true
  })]
});
