project('suckless', 'c')

freetype = dependency('freetype2', required : true)
Xlib = dependency('x11', required : true)
xft = dependency('xft', required : true)
fontconfig = dependency('fontconfig', required : true)

cc = meson.get_compiler('c')
xrender = cc.find_library('Xrender', required : true)
m_dep = cc.find_library('m', required : true)


subdir('common')
subdir('dwm')
subdir('st')
subdir('dmenu')
# subdir('surf')
subdir('dwmblocks')
# subdir('slock')
