import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'
import pluginRewriteAll from 'vite-plugin-rewrite-all';

// https://vitejs.dev/config/

export default defineConfig({
  plugins: [preact(), pluginRewriteAll()],
  base: ''
})
