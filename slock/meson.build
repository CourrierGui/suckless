sources = [
  'slock.c', 'explicit_bzero.c', 'arg.h', 'config.h', 'util.h',
]

version = '1.4'

cc = meson.get_compiler('c')
crypt_dep = cc.find_library('crypt', required : true)
xrandr_dep = cc.find_library('Xrandr', required : true)

executable('slock', sources, install : true,
           dependencies : [ Xlib, crypt_dep, xrandr_dep ],
           c_args : [
             '-DVERSION="' + version + '"',
             '-DHAVE_SHADOW_H', '-std=gnu99'
           ]
)

install_man('slock.1')
