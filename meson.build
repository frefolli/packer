project('packer', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

ts_yaml = dependency('ts-yaml')
curl = dependency('libcurl')
include = include_directories('include')

executable('main.exe', [
  'src/packer/packerfile.c',
  'src/packer/package.c',
  'src/packer/repository.c',
  'src/packer/utility.c',
  'src/packer/main.c'
  ], dependencies: [ts_yaml, curl]
  , include_directories: [include])
