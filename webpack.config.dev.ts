import webpack from "webpack";
import { smart } from "webpack-merge";
import baseConfig, { lessCommonLoaders } from "./webpack.config.base";

export default smart(baseConfig, {
  mode: "development",
  devtool: "inline-source-map",
  module: {
    rules: [
      {
        test: [/\.less$/, /\.css$/],
        use: [
          {
            loader: "style-loader"
          },
          ...lessCommonLoaders
        ]
      }
    ]
  },
  plugins: [
    new webpack.DefinePlugin({
      "process.env": {
        NODE_ENV: JSON.stringify("development")
      }
    })
  ]
});