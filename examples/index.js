'use strict';

const Image = require('../');


const fiLogo = new Image();

fiLogo.src = 'freeimage.jpg';

fiLogo.on('load', src => console.log('load-event', fiLogo));

fiLogo.onload = src => console.log('onload-cb', fiLogo);
