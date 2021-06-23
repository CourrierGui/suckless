sources = [
  'arg.h', 'config.h', 'dmenu.c', 'drw.c', 'drw.h', 'util.c', 'util.h',
]

version = '5.0'

executable('dmenu', sources,
           dependencies : [ freetype, Xlib, xft, fontconfig ],
           c_args : [ '-DVERSION="' + version + '"', '-std=gnu99' ]
)

executable('stest', 'stest.c')
