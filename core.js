'use strict';

// Add deps dll dirs
require('deps-freeimage-raub');

const { binPath } = require('addon-tools-raub');

const core = require(`./${binPath}/image`);


module.exports = core;
