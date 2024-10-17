project('packer', 'cpp',
  version : '0.1',
  default_options : ['werror=true', 'warning_level=3', 'optimization=3', 'buildtype=release'])

include = include_directories('./include')
yaml_cpp = dependency('yaml-cpp', required : false)

libpacker = library('packer', [
    'src/packer/util.cc',
    'src/packer/serde.cc',
    'src/packer/io.cc',
    'src/packer/specialization.cc',
    'src/packer/scheduling.cc',
    'src/packer/analysis.cc',
    'src/packer/locator.cc',
  ], dependencies: [
    yaml_cpp
  ], include_directories: [
    include
  ])
 
executable('packer.exe', [
   'src/packer/main.cc'
  ], dependencies: [
    yaml_cpp
  ], link_with: [
    libpacker
  ], include_directories: [
    include
  ])
