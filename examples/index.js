'use strict';

const Image = require('../');

const dataUri = require('./data-uri')[0];


const fiLogo = new Image();
fiLogo.src = 'freeimage.jpg';
fiLogo.on('load', src => console.log('\nFilesystem loaded', fiLogo));


const fiLogo2 = new Image();
fiLogo2.src = dataUri;
fiLogo2.on('load', src => console.log('\nData Uri loaded', fiLogo2));


const fiLogo3 = new Image();
fiLogo3.src = 'http://freeimage.sourceforge.net/images/logo.jpg';
fiLogo3.on('load', src => console.log('\nHTTP loaded', fiLogo3));
