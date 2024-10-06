project('packer', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

ts_yaml = dependency('ts-yaml')
include = include_directories('include')

executable('main.exe', [
  'src/packer/packerfile.c',
  'src/packer/main.c'
  ], dependencies: [ts_yaml]
  , include_directories: [include])
