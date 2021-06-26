sources = [
  'dwm.c', 'dwm.h', 'drw.c', 'drw.h', 'util.c', 'util.h'
]

version = '6.2'

cc = meson.get_compiler('c')
xrender = cc.find_library('Xrender', required : true)

executable('dwm', sources, install : true,
           dependencies : [ freetype, Xlib, xft, fontconfig, xrender ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=c99', '-Wno-deprecated-declarations'
           ]
)

executable('transient', 'transient.c', dependencies : [ Xlib ], install : true)

install_man('dwm.1')
