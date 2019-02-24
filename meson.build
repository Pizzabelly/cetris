project('cetris', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c99'])

compiler = meson.get_compiler('c')

if host_machine.system() == 'windows'
 curses = compiler.find_library('pdcurses/pdcurses.a', dirs: meson.source_root())
else
 curses = dependency('ncursesw')
endif

executable('cetris', ['cetris.c', 'ncurses_ui.c'], 
	         dependencies: curses,
           install : true)
