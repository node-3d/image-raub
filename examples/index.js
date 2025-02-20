'use strict';

const Image = require('../');

const dataUri = require('./data-uri')[0];


const fiLogo = new Image();
fiLogo.src = 'freeimage.jpg';
fiLogo.on('load', () => console.log('\nFilesystem loaded', fiLogo));


const fiLogo2 = new Image();
fiLogo2.src = dataUri;
fiLogo2.on('load', () => console.log('\nData Uri loaded', fiLogo2));


const fiLogo3 = new Image();
fiLogo3.src = 'http://freeimage.sourceforge.net/images/logo.jpg';
fiLogo3.on('load', () => console.log('\nHTTP loaded', fiLogo3));


(async () => {
	const fiLogo4 = await Image.loadAsync('freeimage.jpg');
	console.log('\nAsync loaded', fiLogo4);
})();