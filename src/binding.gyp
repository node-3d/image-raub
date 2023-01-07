{
	'variables': {
		'bin': '<!(node -p "require(\'addon-tools-raub\').getBin()")',
		'fi_include': '<!(node -p "require(\'deps-freeimage-raub\').include")',
		'fi_bin': '<!(node -p "require(\'deps-freeimage-raub\').bin")',
	},
	'targets': [{
		'target_name': 'image',
		'sources': [
			'cpp/bindings.cpp',
			'cpp/image.cpp',
		],
		'include_dirs': [
			'<(fi_include)',
			'<!@(node -p "require(\'addon-tools-raub\').getInclude()")',
		],
		'defines': ['UNICODE', '_UNICODE'],
		'cflags_cc': ['-std=c++17', '-fno-exceptions'],
		'cflags': ['-fno-exceptions'],
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
				'MACOSX_DEPLOYMENT_TARGET': '10.9',
				'defines': ['__APPLE__'],
				'CLANG_CXX_LIBRARY': 'libc++',
				'OTHER_CFLAGS': ['-std=c++17', '-fno-exceptions'],
			}],
			['OS=="win"', {
				'libraries': ['FreeImage.lib'],
				'defines': ['WIN32_LEAN_AND_MEAN', 'VC_EXTRALEAN', '_WIN32', '_HAS_EXCEPTIONS=0'],
				'msvs_settings' : {
					'VCCLCompilerTool' : {
						'AdditionalOptions' : [
							'/O2','/Oy','/GL','/GF','/Gm-', '/std:c++17',
							'/EHa-s-c-','/MT','/GS','/Gy','/GR-','/Gd',
						]
					},
					'VCLinkerTool' : {
						'AdditionalOptions' : ['/DEBUG:NONE', '/LTCG', '/OPT:NOREF'],
					},
				},
			}],
		],
	}],
}
