project('cetris', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c99'])

compiler = meson.get_compiler('c')

src = ['cetris.c']

if get_option('build_frontends') == true
  if host_machine.system() == 'windows'
   curses = compiler.find_library('pdcurses/pdcurses.a', dirs: meson.source_root())
  else
   curses = dependency('ncursesw')
  endif
  src += ['frontend/ncurses/ncurses_ui.c']
endif

if get_option('debug') == true 
  src += ['test.c']
  add_global_arguments('-DBUILD_TESTS', language : 'c') 
endif

executable('cetris', src, 
	         dependencies: curses,
           install : true)
