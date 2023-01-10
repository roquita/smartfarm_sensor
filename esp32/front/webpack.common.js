const path = require('path');
const HtmlWebpackPlugin = require('html-webpack-plugin');
const glob = require('glob')
const MiniCssExtractPlugin = require('mini-css-extract-plugin')
const PurgeCSSPlugin = require('purgecss-webpack-plugin')

const PATHS = {
    src: path.join(__dirname, 'src')
}

module.exports = {

    entry: {
        index: "./src/main/menu/menu.js"
    },

    output: {
        path: __dirname + "/build",
        filename: "js/[name].js",
        clean: true,
    },

    plugins: [

        new HtmlWebpackPlugin({
            chunks: ['index'],
            template: "./src/main/main.html",
            filename: '[name].html',
            title: 'Main common',
            favicon: "./src/favicon.png"
        }),

        new MiniCssExtractPlugin({
            filename: "css/[name].css",
        }),

        new PurgeCSSPlugin({
            paths: glob.sync(`${PATHS.src}/**/*`, { nodir: true }),
        }),

    ],

    module: {
        rules: [{
                test: /\.css$/,
                use: [
                    MiniCssExtractPlugin.loader,
                    { loader: "css-loader" },
                ],
            },
            {
                test: /\.scss$/,
                use: [
                    MiniCssExtractPlugin.loader,
                    { loader: "css-loader" },
                    { loader: "sass-loader" },
                ],
            },
            {
                test: /\.(png|svg|jpg|jpeg|gif)$/i,
                type: 'asset/resource',
            },
            {
                test: /\.(woff|woff2|eot|ttf|otf)$/i,
                type: 'asset/resource',
            },
            {
                test: /\.(csv|tsv)$/i,
                use: ['csv-loader'],
            },
            {
                test: /\.xml$/i,
                use: ['xml-loader'],
            },
        ],
    },
};