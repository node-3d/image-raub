import { fileURLToPath } from 'node:url';
import { Image } from '@node-3d/image';
import { dataUri } from './data-uri.ts';

const freeimagePath = fileURLToPath(new URL('assets/freeimage.jpg', import.meta.url));

const fiLogo = new Image();
fiLogo.src = freeimagePath;
fiLogo.on('load', () => {
	console.log('\nFilesystem loaded', fiLogo);
});

const fiLogo2 = new Image();
fiLogo2.src = dataUri;
fiLogo2.on('load', () => {
	console.log('\nData Uri loaded', fiLogo2);
});

const fiLogo3 = new Image();
fiLogo3.src = 'https://raw.githubusercontent.com/node-3d/node-3d/master/logo/front/logo.png';
fiLogo3.on('load', () => {
	console.log('\nHTTP loaded', fiLogo3);
});

const fiLogo4 = await Image.loadAsync(freeimagePath);
console.log('\nAsync loaded', fiLogo4);
