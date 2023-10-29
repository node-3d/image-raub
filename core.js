'use strict';

require('segfault-raub');

// Add deps dll dirs
require('deps-freeimage-raub');

const { getBin } = require('addon-tools-raub');

const core = require(`./${getBin()}/image`);

module.exports = core;
