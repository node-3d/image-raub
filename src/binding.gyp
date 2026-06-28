{
	'variables': {
		'bin': '<!(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printBin())")',
		'fi_include': '<!(node -p "require(\'@node-3d/deps-freeimage\').include")',
		'fi_bin': '<!(node -p "require(\'@node-3d/deps-freeimage\').bin")',
	},
	'targets': [{
		'target_name': 'image',
		'includes': ['common.gypi'],
		'sources': [
			'cpp/bindings.cpp',
			'cpp/image.cpp',
		],
		'include_dirs': [
			'<(fi_include)',
			'<!@(node -e "import(\'@node-3d/addon-tools\').then((m) => m.printInclude())")',
		],
		'library_dirs': ['<(fi_bin)'],
		'conditions': [
			['OS=="linux"', {
				'libraries': [
					"-Wl,-rpath,'$$ORIGIN'",
					"-Wl,-rpath,'$$ORIGIN/../node_modules/@node-3d/deps-freeimage/<(bin)'",
					"-Wl,-rpath,'$$ORIGIN/../../@node-3d/deps-freeimage/<(bin)'",
					'<(fi_bin)/libfreeimage.so.3',
				],
			}],
			['OS=="mac"', {
				'xcode_settings': { 'DYLIB_INSTALL_NAME_BASE': '@rpath' },
				'libraries': [
					'-Wl,-rpath,@loader_path',
					'-Wl,-rpath,@loader_path/../node_modules/@node-3d/deps-freeimage/<(bin)',
					'-Wl,-rpath,@loader_path/../../@node-3d/deps-freeimage/<(bin)',
					'<(fi_bin)/freeimage.dylib',
				],
			}],
			['OS=="win"', {
				'libraries': ['FreeImage.lib'],
			}],
		],
	}],
}
