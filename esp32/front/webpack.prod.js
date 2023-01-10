const { merge } = require('webpack-merge');
const common = require('./webpack.common.js');

const CssMinimizerPlugin = require('css-minimizer-webpack-plugin');
const CompressionPlugin = require("compression-webpack-plugin");

module.exports = merge(common, {

    mode: 'production',

    //devtool: 'source-map',

    optimization: {

        minimizer: [
            `...`,
            new CssMinimizerPlugin(),
        ],

    },

    plugins: [

        new CompressionPlugin({
            exclude: /.map$/,
            deleteOriginalAssets: 'keep-source-map',
            filename: '[path][base].gzip',
            algorithm: 'gzip',
            test: /\.js$|\.css$|\.html$|\.eot?.+$|\.ttf?.+$|\.woff?.+$|\.svg?.+$|\.png?.+$/,
            threshold: 0,
            minRatio: 0.8
        })

    ]
});