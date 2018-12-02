{
	'variables': {
		'rm'                : '<!(node -e "require(\'addon-tools-raub\').rm()")',
		'cp'                : '<!(node -e "require(\'addon-tools-raub\').cp()")',
		'mkdir'             : '<!(node -e "require(\'addon-tools-raub\').mkdir()")',
		'binary'            : '<!(node -e "require(\'addon-tools-raub\').bin()")',
		'freeimage_include' : '<!(node -e "require(\'deps-freeimage-raub\').include()")',
		'freeimage_bin'     : '<!(node -e "require(\'deps-freeimage-raub\').bin()")',
	},
	'targets': [
		{
			'target_name': 'image',
			'sources': [
				'cpp/bindings.cpp',
				'cpp/image.cpp',
			],
			'include_dirs': [
				'<!@(node -e "require(\'addon-tools-raub\').include()")',
				'<(freeimage_include)',
				'<(module_root_dir)/include',
			],
			'library_dirs': [ '<(freeimage_bin)' ],
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
							'-dylib_file libfreeimage.dylib:<(freeimage_bin)/freeimage.dylib',
							'-dylib_file libfreeimage-3.17.0.dylib-x86_64:<(freeimage_bin)/freeimage.dylib',
							'<(freeimage_bin)/freeimage.dylib',
						],
					}
				],
				[
					'OS=="win"',
					{
						'libraries': [ 'FreeImage.lib' ],
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
		{
			'target_name'  : 'make_directory',
			'type'         : 'none',
			'dependencies' : ['image'],
			'actions'      : [{
				'action_name' : 'Directory created.',
				'inputs'      : [],
				'outputs'     : ['build'],
				'action': ['<(mkdir)', '-p', '<(binary)']
			}],
		},
		{
			'target_name'  : 'copy_binary',
			'type'         : 'none',
			'dependencies' : ['make_directory'],
			'actions'      : [{
				'action_name' : 'Module copied.',
				'inputs'      : [],
				'outputs'     : ['binary'],
				'action'      : ['<(cp)', 'build/Release/image.node', '<(binary)/image.node'],
			}],
		},
		{
			'target_name'  : 'remove_extras',
			'type'         : 'none',
			'dependencies' : ['copy_binary'],
			'actions'      : [{
				'action_name' : 'Build intermediates removed.',
				'inputs'      : [],
				'outputs'     : ['cpp'],
				'conditions'  : [
					[ 'OS=="linux"', { 'action' : [
						'rm',
						'<(module_root_dir)/build/Release/obj.target/image/cpp/image.o',
						'<(module_root_dir)/build/Release/obj.target/image.node',
						'<(module_root_dir)/build/Release/image.node'
					] } ],
					[ 'OS=="mac"', { 'action' : [
						'rm',
						'<(module_root_dir)/build/Release/obj.target/image/cpp/image.o',
						'<(module_root_dir)/build/Release/image.node'
					] } ],
					[ 'OS=="win"', { 'action' : [
						'<(rm)',
						'<(module_root_dir)/build/Release/image.*',
						'<(module_root_dir)/build/Release/obj/image/*.*'
					] } ],
				],
			}],
		},
		
	]
}
