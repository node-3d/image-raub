'use strict';

require('segfault-raub');

// Add deps dll dirs
require('deps-freeimage-raub');

const { bin } = require('addon-tools-raub');

const core = require(`./${bin}/image`);


module.exports = core;
