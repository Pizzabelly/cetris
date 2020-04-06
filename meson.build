project('cetris', 'c',
  version : '0.3',
  default_options : ['warning_level=2', 'c_std=c99'])

compiler = meson.get_compiler('c')

is_windows = host_machine.system() == 'windows'
root_dir = meson.current_source_dir()

cetris_inc = include_directories(['lib', 'ini'])

if get_option('debug')
  if is_windows
    add_global_arguments('/DBUILD_TESTS', language : 'c') 
  else 
    add_global_arguments('-DBUILD_TESTS', language : 'c')
  endif
endif

if get_option('debug')
  subdir('test')
endif

if get_option('curses-frontend')
  subdir('frontends/curses')
endif

if get_option('gl-frontend')
  subdir('frontends/gl')
endif

if get_option('sdl-frontend')
  subdir('frontends/sdl')
endif
