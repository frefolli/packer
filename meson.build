project('packer', 'c',
  version : '0.1',
  default_options : ['werror=true', 'warning_level=3'])

ts_yaml = dependency('ts-yaml')
curl = dependency('libcurl')
include = include_directories('include')

libpacker = library('packer', [
  'src/packer/packerfile.c',
  'src/packer/package.c',
  'src/packer/repository.c',
  'src/packer/utility.c',
  ], dependencies: [ts_yaml, curl]
  , include_directories: [include])
 
executable('main.exe', [
 'src/packer/main.c'
  ], link_with: [libpacker]
  , include_directories: [include])

test('test_packerfile',
  executable('test_packerfile',
    'tests/packerfile.c',
    link_with: [libpacker],
    include_directories: [include]))
