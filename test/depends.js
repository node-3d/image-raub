'use strict';

const path = require('node:path');
const util = require('node:util');
const exec = util.promisify(require('node:child_process').exec);

const { copy } = require('addon-tools-raub');

require('deps-freeimage-raub');

(async () => {
	try {
		console.log('DEPENDS');
		await copy(path.resolve('../bin-windows/image.node'), path.resolve('./image.node'));
		
		const { stderr } = await exec('depends /c /u:1 /pl:1 /pf:1 /ot:dep.txt image.node');
		if (stderr) {
			console.error(stderr);
		}
		
		console.log('-------------------');
	} catch (error) {
		console.error(error);
	}
})();
