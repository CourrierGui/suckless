deps = [
  dependency('gtk+-3.0', required : true),
  dependency('gcr-3', required : true, method : 'pkg-config'),
  dependency('webkit2gtk-4.0', required : true),
]

webkit = [
  dependency('webkit2gtk-4.0', required : true),
  dependency('webkit2gtk-web-extension-4.0', required : true),
  dependency('gio-2.0', required : true),
]

version = '2.1'

surf_sources = [
  'surf.c', 'arg.h', 'common.h', 'config.h',
]

webext_sources = [
  'webext-surf.c', 'arg.h', 'common.h', 'config.h',
]

executable('surf', surf_sources, install : true,
           dependencies : [ Xlib, deps ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=gnu99', '-DGCR_API_SUBJECT_TO_CHANGE',
             '-DWEBEXTDIR="' + '"', '-DLIBPREFIX="' + '"',
           ]
)

library('webext-surf', webext_sources, install : true,
           dependencies : [ Xlib, webkit ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=gnu99'
           ]
)

custom_target('surf open',
  input : 'surf-open.sh',
  output :  'surf-open.sh',
  command : ['cp', '@INPUT@', '@OUTPUT@'],
  install : true,
  install_dir : get_option('bindir'),
  build_by_default : true
)


install_man('surf.1')
