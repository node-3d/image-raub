{
	'variables': {
		'bin': '<!(node -p "require(\'addon-tools-raub\').getBin()")',
		'fi_include': '<!(node -p "require(\'deps-freeimage-raub\').include")',
		'fi_bin': '<!(node -p "require(\'deps-freeimage-raub\').bin")',
	},
	'targets': [{
		'target_name': 'image',
		'includes': ['../node_modules/addon-tools-raub/utils/common.gypi'],
		'sources': [
			'cpp/bindings.cpp',
			'cpp/image.cpp',
		],
		'include_dirs': [
			'<(fi_include)',
			'<!@(node -p "require(\'addon-tools-raub\').getInclude()")',
		],
		'library_dirs': ['<(fi_bin)'],
		'conditions': [
			['OS=="linux"', {
				'libraries': [
					"-Wl,-rpath,'$$ORIGIN'",
					"-Wl,-rpath,'$$ORIGIN/../node_modules/deps-freeimage-raub/<(bin)'",
					"-Wl,-rpath,'$$ORIGIN/../../deps-freeimage-raub/<(bin)'",
					'<(fi_bin)/libfreeimage.so.3',
				],
			}],
			['OS=="mac"', {
				'xcode_settings': { 'DYLIB_INSTALL_NAME_BASE': '@rpath' },
				'libraries': [
					'-Wl,-rpath,@loader_path',
					'-Wl,-rpath,@loader_path/../node_modules/deps-freeimage-raub/<(bin)',
					'-Wl,-rpath,@loader_path/../../deps-freeimage-raub/<(bin)',
					'<(fi_bin)/freeimage.dylib',
				],
			}],
			['OS=="win"', {
				'libraries': ['FreeImage.lib'],
			}],
		],
	}],
}
