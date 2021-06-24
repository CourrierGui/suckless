sources = [
  'arg.h', 'config.h', 'st.c', 'st.h', 'win.h', 'x.c',
]

version = '0.8.4'

cc = meson.get_compiler('c')
# ceil
m_dep = cc.find_library('m', required : true)
# openpty
util_dep = cc.find_library('util', required : true)

executable('st', sources, install : true,
           dependencies : [ freetype, Xlib, xft, fontconfig, m_dep, util_dep ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=gnu99'
           ]
)

install_man('st.1')
