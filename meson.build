project('cetris', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c99'])

compiler = meson.get_compiler('c')

src = ['cetris.c']

if get_option('debug') == true 
  src += ['test.c']
  add_global_arguments('-DBUILD_TESTS', language : 'c') 
endif

if get_option('build_frontends') == true
  if host_machine.system() == 'windows'
   curses = compiler.find_library('pdcurses/pdcurses.a', dirs: meson.source_root())
  else
   curses = dependency('ncursesw')
  endif
  sdl = dependency('sdl2')
  sdl2_ttf = meson.get_compiler('c').find_library('SDL2_ttf')
  executable('cetris_curses', [src, 'frontend/ncurses/ncurses_ui.c'], 
	       dependencies: curses,
           install : false)
  executable('cetris_sdl', [src, 'frontend/sdl/sdl_ui.c'], 
	       dependencies: [sdl, sdl2_ttf],
           install : false)
endif
