const colors = require('tailwindcss/colors')

module.exports = {
  mode: 'jit',
  purge: [
    './src/**/*.html',
    './src/**/*.jsx',
    './src/**/*.tsx',
  ],
  darkMode: false, // or 'media' or 'class'
  theme: {
    colors: {
      transparent: 'transparent',
      current: 'currentColor',
      black: colors.black,
      white: colors.white,
      gray: colors.trueGray,
      green: colors.green,
      grey: {
        DEFAULT: '#4f4f4f',
        light: '#5B5B5B'
      },
      indigo: colors.indigo,
      red: colors.rose,
      yellow: colors.amber,
      
    }
  },
  variants: {
    extend: {},
  },
  plugins: [],
}
