sources = [
  'dwm.c', 'dwm.h', 'drw.c', 'drw.h',
]

version = '6.2'

cc = meson.get_compiler('c')
xcb = [
  cc.find_library('X11-xcb', required : true),
  cc.find_library('xcb', required : true),
  cc.find_library('xcb-res', required : true),
]

executable('dwm', sources, install : true,
           dependencies : [ freetype, Xlib, xft, fontconfig, xrender, xcb, common_dep ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=gnu99', '-Wno-deprecated-declarations'
           ]
)

executable('transient', 'transient.c', dependencies : [ Xlib, common_dep ], install : true)

install_man('dwm.1')
