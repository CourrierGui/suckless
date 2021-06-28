sources = [
  'st.c', 'st.h', 'utils.h', 'win.h',
  'x.c', 'x.h', 'arg.h', 'config.h',
  'boxdraw.c', 'boxdraw_data.h',
  'hb.c', 'hb.h',
  'normalMode.c', 'normalMode.h',
]

version = '0.8.4'

cc = meson.get_compiler('c')
# openpty
util_dep = cc.find_library('util', required : true)
harfbuzz = dependency('harfbuzz', required : true)

executable('st', sources, install : true,
           dependencies : [
             freetype, Xlib, xft, fontconfig, m_dep, util_dep, harfbuzz, common_dep
           ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-std=gnu99'
           ]
)

install_man('st.1')
