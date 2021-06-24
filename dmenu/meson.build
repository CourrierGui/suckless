sources = [
  'arg.h', 'config.h', 'dmenu.c', 'drw.c', 'drw.h', 'util.c', 'util.h',
]

version = '5.0'

executable('dmenu', sources, install : true,
           dependencies : [ freetype, Xlib, xft, fontconfig ],
           c_args : [ '-DVERSION="' + version + '"', '-std=gnu99' ]
)

executable('stest', 'stest.c', install : true)

custom_target('dmenu_path',
  input : 'dmenu_path',
  output :  'dmenu_path',
  command : ['cp', '@INPUT@', '@OUTPUT@'],
  install : true,
  install_dir : get_option('bindir'),
  build_by_default : true
)

custom_target('dmenu_run',
  input : 'dmenu_run',
  output :  'dmenu_run',
  command : ['cp', '@INPUT@', '@OUTPUT@'],
  install : true,
  install_dir : get_option('bindir'),
  build_by_default : true
)

install_man('dmenu.1', 'stest.1')
