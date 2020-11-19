import BabelMinifyPlugin from "babel-minify-webpack-plugin";
import { CleanWebpackPlugin } from "clean-webpack-plugin";
import MiniCssExtractPlugin from "mini-css-extract-plugin";
import OptimizeCSSAssetsPlugin from "optimize-css-assets-webpack-plugin";
import { smart } from "webpack-merge";
import baseConfig, { lessCommonLoaders } from "./webpack.config.base";
import WebappWebpackPlugin from "webapp-webpack-plugin";
import path from "path";
import webpack = require("webpack");

export default smart(baseConfig, {
  mode: "production",
  optimization: {
    minimizer: [
      new BabelMinifyPlugin({}) as any,
      new OptimizeCSSAssetsPlugin({})
    ]
  },
  module: {
    rules: [
      {
        test: /\.svg$/,
        use: [
          {
            loader: "image-webpack-loader",
            options: {}
          }
        ]
      },
      {
        test: [/\.less$/, /\.css$/],
        use: [
          "ignore-loader",
          {
            loader: MiniCssExtractPlugin.loader
          },
          ...lessCommonLoaders
        ]
      }
    ]
  },
  plugins: [
    new MiniCssExtractPlugin({
      // Options similar to the same options in webpackOptions.output
      // both options are optional
      filename: "static/[name].[hash].css",
      chunkFilename: "static/[id].[hash].css"
    }),
    new CleanWebpackPlugin(),
    // new WebappWebpackPlugin({
    //   logo: path.join(__dirname, "src/resources/logo.png"),
    //   prefix: "static/"
    // }),
    new webpack.DefinePlugin({
      "process.env.READABILITY_PROXY_URL": JSON.stringify(
        process.env.READABILITY_PROXY_URL
      )
    })
  ]
});
