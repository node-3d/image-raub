'use strict';

const path = require('node:path');
const util = require('node:util');
const exec = util.promisify(require('node:child_process').exec);

const {
	copy,
} = require('addon-tools-raub');

const fail = (error) => {
	console.error(error);
	process.exit(-1);
};


(async () => {
	try {
		console.log('DEPENDS');
		await copy(path.resolve('../bin-windows/image.node'), path.resolve('./image.node'));
		
		const { stderr } = await exec('depends /c /u:1 /pl:1 /pf:1 /ot:dep.txt image.node');
		if (stderr) {
			console.error(stderr);
		}
		
		// // eslint-disable-next-line no-constant-condition
		// while (true) {
		// 	await new Promise((res) => setTimeout(res, 3000));
		// 	const isFound = await exists('dep.txt');
			
		// 	console.log('isFound:', isFound);
			
		// 	if (isFound) {
		// 		break;
		// 	}
		// }
		
		console.log('-------------------');
	} catch (error) {
		fail(error);
	}
})();
