import Image from '../index.js';
import { dataUri } from './data-uri.mjs';


const fiLogo = new Image();
fiLogo.src = 'freeimage.jpg';
fiLogo.on('load', () => console.log('\nFilesystem loaded', fiLogo));


const fiLogo2 = new Image();
fiLogo2.src = dataUri;
fiLogo2.on('load', () => console.log('\nData Uri loaded', fiLogo2));


const fiLogo3 = new Image();
fiLogo3.src = 'https://raw.githubusercontent.com/node-3d/node-3d/master/logo/front/logo.png';
fiLogo3.on('load', () => console.log('\nHTTP loaded', fiLogo3));


(async () => {
	const fiLogo4 = await Image.loadAsync('freeimage.jpg');
	console.log('\nAsync loaded', fiLogo4);
})();