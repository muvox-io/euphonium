import HtmlWebpackPlugin from "html-webpack-plugin";
import { Plugin as IconFontPlugin } from "icon-font-loader";
import path from "path";
import webpack from "webpack";


const cssModulesNamePattern = "[sha1:hash:hex:4]";
export const cssCommonLoaders = [
  {
    loader: "css-loader", // translates CSS into CommonJS,
    options: {
      modules: {
        localIdentName: cssModulesNamePattern
      }
    }
  },
  "icon-font-loader"
];

export const lessCommonLoaders = [
  ...cssCommonLoaders,
  {
    loader: "less-loader" // compiles Less to CSS
  }
];

const context = path.resolve(__dirname, "src");

const config: webpack.Configuration = {
  mode: "production",
  entry: "./index.tsx",
  context,
  devServer: {
    historyApiFallback: {
      index: "/index.html"
    },
    disableHostCheck: true
  },
  output: {
    path: path.resolve(__dirname, "dist"),
    filename: "static/app.[hash].js",
    publicPath: "/"
  },
  resolve: {
    // Add `.ts` and `.tsx` as a resolvable extension.
    extensions: [".ts", ".tsx", ".js"],
    alias: {
      react: "preact-compat",
      "react-dom": "preact-compat"
    }
  },
  optimization: {
    splitChunks: {
      chunks: "initial"
    }
  },
  module: {
    rules: [
      // all files with a `.ts` or `.tsx` extension will be handled by `ts-loader`
      {
        test: /\.tsx?$/,
        use: [
          {
            loader: "babel-loader",
            query: {
              plugins: [
                [
                  "@babel/transform-react-jsx",
                  {
                    pragma: "h",
                    pragmaFrag: "Fragment"
                  }
                ],
                [
                  "react-css-modules",
                  {
                    context,
                    generateScopedName: cssModulesNamePattern,
                    filetypes: {
                      ".less": {
                        syntax: "postcss-less"
                      }
                    },
                    attributeNames: { styleName: "class" }
                  }
                ],
                [
                  "auto-import", // we use this plugin insstead of webpack.ProvidePlugin to support better module concatenation
                  {
                    declarations: [
                      { members: ["h", "Fragment"], path: "preact" }
                    ]
                  }
                ]
              ]
            }
          },

          { loader: "ts-loader" }
        ]
      },
      {
        test: [/\.less$/, /\.css$/],
        use: [...lessCommonLoaders]
      },
      {
        test: /\.svg$/,
        use: [{ loader: "url-loader" }]
      }
    ]
  },
  plugins: [
    new HtmlWebpackPlugin({
      title: "HN Offline",
      meta: {
        viewport: "width=device-width, initial-scale=1"
      }
    }),
    //new BundleAnalyzerPlugin(),
    new IconFontPlugin({
      output: "./static/",
      fontName: "icn",
      filename: "[name].[ext]?[sha1:hash:hex:5]",
      types: ["woff", "eot", "ttf", "svg"]
    })
  ]
};

export default config;