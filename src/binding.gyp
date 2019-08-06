{
	'variables': {
		'rm'                : '<!(node -p "require(\'addon-tools-raub\').rm")',
		'cp'                : '<!(node -p "require(\'addon-tools-raub\').cp")',
		'mkdir'             : '<!(node -p "require(\'addon-tools-raub\').mkdir")',
		'binary'            : '<!(node -p "require(\'addon-tools-raub\').bin")',
		'freeimage_include' : '<!(node -p "require(\'deps-freeimage-raub\').include")',
		'freeimage_bin'     : '<!(node -p "require(\'deps-freeimage-raub\').bin")',
	},
	'targets': [
		{
			'target_name': 'image',
			'sources': [
				'cpp/bindings.cpp',
				'cpp/image.cpp',
			],
			'include_dirs': [
				'<(freeimage_include)',
				'<!@(node -p "require(\'addon-tools-raub\').include")',
			],
			'cflags!': ['-fno-exceptions'],
			'cflags_cc!': ['-fno-exceptions'],
			'library_dirs': ['<(freeimage_bin)'],
			'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
			'conditions': [
				[
					'OS=="linux"',
					{
						'libraries': [
							'-Wl,-rpath,<(freeimage_bin)',
							'<(freeimage_bin)/libfreeimage.so.3',
						],
					}
				],
				[
					'OS=="mac"',
					{
						'libraries': [
							'-Wl,-rpath,<(freeimage_bin)',
							'<(freeimage_bin)/freeimage.dylib',
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
