project('cetris', 'c',
  version : '0.1',
  default_options : ['warning_level=3', 'c_std=c99'])

compiler = meson.get_compiler('c')

src = ['cetris.c']

if host_machine.system() != 'windows'
  if get_option('debug') == true 
    add_global_arguments('-DBUILD_TESTS', language : 'c') 
  endif
endif

src += ['test.c']
sdlinc = []
sdlttfinc = []

if get_option('build_frontends') == true
  if host_machine.system() == 'windows'
   sdlinc += include_directories('_win\SDL2-2.0.5\include')
   sdlttfinc += include_directories('_win\SDL2_ttf-2.0.14\include')
   curses = compiler.find_library('pdcurses', dirs: [meson.source_root() + '/_win/pdcurses'])
   sdl2 = compiler.find_library('SDL2', dirs: [meson.source_root() + '/_win/SDL2-2.0.5/lib/x86'])
   sdl2_ttf = compiler.find_library('SDL2_ttf', dirs: [meson.source_root() + '/_win/SDL2_ttf-2.0.14/lib/x86'])
  else
   curses = dependency('ncursesw')
   sdl2 = dependency('sdl2')
   sdl2_ttf = meson.get_compiler('c').find_library('SDL2_ttf')
  endif
#  executable('cetris_curses', [src, 'frontend/ncurses/ncurses_ui.c'], 
#	       dependencies: curses,
#           install : false)
  executable('cetris_sdl', [src, 'frontend/sdl/sdl_ui.c'], 
         include_directories: [sdlttfinc, sdlinc],
	       dependencies: [sdl2, sdl2_ttf],
           install : false)
endif
