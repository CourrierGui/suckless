sources = [
  'sl-utils.h', 'utils.c',
  'sl-draw.h', 'draw.c',
]

common_headers = include_directories('.')
common_lib = library('common', sources, dependencies : [ Xlib, xft, freetype, fontconfig ])
common_dep = declare_dependency(link_with : common_lib, include_directories : common_headers)
