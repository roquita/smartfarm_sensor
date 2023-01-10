const { merge } = require('webpack-merge');
const common = require('./webpack.common.js');
const path = require('path');

module.exports = merge(common, {

    mode: 'development',

    devtool: 'inline-source-map',

    devServer: {
        //contentBase: __dirname + "/build",
        static: {
            directory: path.join(__dirname, 'src'),
            serveIndex: true,
        },
        compress: true,
        port: 9000,

    },

});