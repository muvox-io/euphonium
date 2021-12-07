const colors = require("tailwindcss/colors");

module.exports = {
  mode: "jit",
  purge: ["./src/**/*.html", "./src/**/*.jsx", "./src/**/*.tsx"],
  darkMode: "class", // or 'media' or 'class'
  theme: {
    colors: {
      transparent: "transparent",
      app: {
        secondary: "var(--secondary-color)",
        "secondary-dark": "var(--secondary-dark)",
        border: "var(--border-color)",
        primary: "var(--primary-color)",
      },
      "app-text": {
        primary: "var(--text-primary)",
        secondary: "var(--text-secondary)",
      },
      current: "currentColor",
      black: colors.black,
      white: colors.white,
      gray: colors.trueGray,
      green: colors.green,
      grey: {
        DEFAULT: "#4f4f4f",
        light: "#5B5B5B",
      },
      indigo: colors.indigo,
      red: colors.rose,
      yellow: colors.amber,
    },
    borderWidth: {
      DEFAULT: "0.5px",
    },
  },
  variants: {
    extend: {},
  },
  plugins: [],
};
