{
	'variables': {
		'_del'              : '<!(node -e "console.log(require(\'node-addon-tools-raub\')._del)")',
		'_rd'               : '<!(node -e "console.log(require(\'node-addon-tools-raub\')._rd)")',
		'freeimage_include' : '<!(node -e "console.log(require(\'node-deps-freeimage-raub\').include)")',
		'freeimage_bin'     : '<!(node -e "console.log(require(\'node-deps-freeimage-raub\').bin)")',
	},
	'targets': [
		{
			'target_name': 'image',
			'sources': [
				'cpp/bindings.cpp',
				'cpp/image.cpp',
			],
			'include_dirs': [
				'<!(node -e "require(\'nan\')")',
				'<!(node -e "console.log(require(\'node-addon-tools-raub\').include)")',
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
							'<(freeimage_bin)/libfreeimage.so',
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
				'conditions'  : [
					[ 'OS=="linux"', { 'action': ['mkdir', '-p', 'binary'] } ],
					[ 'OS=="mac"', { 'action': ['mkdir', '-p', 'binary'] } ],
					[ 'OS=="win"', { 'action': [
						'<(_rd) "<(module_root_dir)/binary" && ' +
						'md "<(module_root_dir)/binary"'
					] } ],
				],
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
				'conditions'  : [
					[ 'OS=="linux"', { 'action' : [
						'cp',
						'<(module_root_dir)/build/Release/image.node',
						'<(module_root_dir)/binary/image.node'
					] } ],
					[ 'OS=="mac"', { 'action' : [
						'cp',
						'<(module_root_dir)/build/Release/image.node',
						'<(module_root_dir)/binary/image.node'
					] } ],
					[ 'OS=="win"', { 'action' : [
						'copy "<(module_root_dir)/build/Release/image.node"' +
						' "<(module_root_dir)/binary/image.node"'
					] } ],
				],
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
						'<(_del) "<(module_root_dir)/build/Release/image.*" && ' +
						'<(_del) "<(module_root_dir)/build/Release/obj/image/*.*"'
					] } ],
				],
			}],
		},
		
	]
}
