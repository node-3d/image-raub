{
	'variables': {
		'bin'        : '<!(node -p "require(\'addon-tools-raub\').bin")',
		'fi_include' : '<!(node -p "require(\'deps-freeimage-raub\').include")',
		'fi_bin'     : '<!(node -p "require(\'deps-freeimage-raub\').bin")',
	},
	'targets': [
		{
			'target_name': 'image',
			'sources': [
				'cpp/bindings.cpp',
				'cpp/image.cpp',
			],
			'include_dirs': [
				'<(fi_include)',
				'<!@(node -p "require(\'addon-tools-raub\').include")',
			],
			'cflags!': ['-fno-exceptions'],
			'cflags_cc!': ['-fno-exceptions'],
			'library_dirs': ['<(fi_bin)'],
			'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
			'conditions': [
				[
					'OS=="linux"',
					{
						'libraries': [
							'-Wl,-rpath,@loader_path',
							'-Wl,-rpath,@loader_path/../node_modules/deps-freeimage-raub/<(bin)',
							'-Wl,-rpath,@loader_path/../../deps-freeimage-raub/<(bin)',
							'<(fi_bin)/libfreeimage.so.3',
						],
					}
				],
				[
					'OS=="mac"',
					{
						'libraries': [
							'-Wl,-rpath,@loader_path',
							'-Wl,-rpath,@loader_path/../node_modules/deps-freeimage-raub/<(bin)',
							'-Wl,-rpath,@loader_path/../../deps-freeimage-raub/<(bin)',
							'<(fi_bin)/freeimage.dylib',
						],
						'xcode_settings': {
							'DYLIB_INSTALL_NAME_BASE': '@rpath',
						},
					}
				],
				[
					'OS=="win"',
					{
						'libraries': ['FreeImage.lib'],
						'defines' : [
							'WIN32_LEAN_AND_MEAN',
							'VC_EXTRALEAN'
						],
						'msvs_version'  : '2013',
						'msvs_settings' : {
							'VCCLCompilerTool' : {
								'AdditionalOptions' : [
									'/O2','/Oy','/GL','/GF','/Gm-','/EHsc',
									'/MT','/GS','/Gy','/GR-','/Gd',
								]
							},
							'VCLinkerTool' : {
								'AdditionalOptions' : ['/OPT:REF','/OPT:ICF','/LTCG']
							},
						},
					}
				],
			],
		},
	]
}
