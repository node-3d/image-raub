'use strict';

require('segfault-raub');

// Add deps dll dirs
require('deps-freeimage-raub');

console.log('dd', process.env.PATH);
console.log('dd2', __dirname);

const { getBin } = require('addon-tools-raub');

const core = require(`./${getBin()}/image`);


module.exports = core;
