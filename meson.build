project('cetris', 'c',
  version : '0.2',
  default_options : ['warning_level=2', 'c_std=c99'])

compiler = meson.get_compiler('c')

cetris_inc = include_directories(['lib', 'ini'])

if get_option('debug') == true 
  if host_machine.system() == 'windows'
    add_global_arguments('/DBUILD_TESTS', language : 'c') 
  else 
    add_global_arguments('-DBUILD_TESTS', language : 'c')
  endif

  subdir('test')
endif

if get_option('curses-frontend') == true
  subdir('frontends/curses')
endif

if get_option('gl-frontend') == true
  subdir('frontends/gl')
endif

if get_option('sdl-frontend') == true
  subdir('frontends/sdl')
endif
