{
	"targets": [
		{
			"target_name": "fallocate",
			"sources": ["fallocate.cc"],
			"include_dirs": [
				"<!(node -e \"require('nan')\")"
			],
			"cflags":[
				"-Wno-cast-function-type"
			],
			"msvs_settings": {
				"VCCLCompilerTool": {
				}
			},
			"xcode_settings": {
				"OTHER_CPLUSPLUSFLAGS": [
				]
			}
		}
	]
}

