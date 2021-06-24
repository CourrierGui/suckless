sources = [
  'dwm.c', 'drw.c', 'drw.h', 'util.c', 'util.h'
]

version = '6.2'

executable('dwm', sources, install : true,
           dependencies : [ freetype, Xlib, xft, fontconfig ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=c99', '-Wno-deprecated-declarations'
           ]
)

executable('transient', 'transient.c', dependencies : [ Xlib ], install : true)

install_man('dwm.1')
