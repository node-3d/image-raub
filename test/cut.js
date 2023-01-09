'use strict';

const { read, write } = require('addon-tools-raub');


(async () => {
	try {
		console.log('CUT');
		let txt = await read('dep.txt');
		
		txt = txt.replace(/^\s{32}.*\r?\n/gm, '');
		txt = txt.replace(/^\s{32}.*/gm, '');
		
		txt = txt.replace(/^.*(KERNEL32|NTDLL|ADVAPI32)\.DLL.*\r?\n/gm, '');
		txt = txt.replace(/^.*(KERNEL32|NTDLL|ADVAPI32)\.DLL.*/gm, '');
		
		txt = txt.replace(/^.*?[A-Z]+-MS-[A-Z]+.*\r?\n/gm, '');
		txt = txt.replace(/^.*?[A-Z]+-MS-[A-Z]+.*/gm, '');
		
		txt = txt.replace(/\\/g, '/');
		
		await write('dep-cut.txt', txt);
	} catch (error) {
		console.error(error);
	}
})();
