sources = [
  'sl-utils.h', 'utils.c',
]

common_headers = include_directories('.')
common_lib = library('common', sources)
common_dep = declare_dependency(link_with : common_lib, include_directories : common_headers)
