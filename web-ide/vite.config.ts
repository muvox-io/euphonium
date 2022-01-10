import { defineConfig } from 'vite'
import preact from '@preact/preset-vite'

// https://vitejs.dev/config/
import alias from '@rollup/plugin-alias';

export default defineConfig({
  plugins: [preact()],
  base: '',
  // build: {
  //   rollupOptions: {
  //     plugins: [
  //       alias({
  //         entries: [
  //           { find: 'react', replacement: 'preact/compat' },
  //           { find: 'react-dom/test-utils', replacement: 'preact/test-utils' },
  //           { find: 'react-dom', replacement: 'preact/compat' },
  //           { find: 'react/jsx-runtime', replacement: 'preact/jsx-runtime' }
  //         ]
  //       })
  //     ]
  //   }
  // }
})
