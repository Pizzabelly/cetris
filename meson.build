project('cetris', 'c',
  version : '0.1',
  default_options : ['warning_level=2', 'c_std=c99'])

compiler = meson.get_compiler('c')

cetris_inc = include_directories('core/include')

if get_option('debug') == true 
  if host_machine.system() == 'windows'
    add_global_arguments('/DBUILD_TESTS', language : 'c') 
  else 
    add_global_arguments('-DBUILD_TESTS', language : 'c')
  endif
endif

subdir('core')

if get_option('ncurses-frontend') == true
  subdir('frontends/ncurses')
endif

if get_option('gl-frontend') == true
  subdir('frontends/opengl')
endif


##if get_option('build_frontends') == true
#  if host_machine.system() == 'windows'
#   sdlinc += include_directories('_win\SDL2-2.0.5\include')
#   sdlinc += include_directories('_win\SDL2_ttf-2.0.14\include')
#   curses = compiler.find_library('pdcurses', dirs: [meson.source_root() + '/_win/pdcurses'])
#   sdl2 = compiler.find_library('SDL2', dirs: [meson.source_root() + '/_win/SDL2-2.0.5/lib/x86'])
#   sdl2_ttf = compiler.find_library('SDL2_ttf', dirs: [meson.source_root() + '/_win/SDL2_ttf-2.0.14/lib/x86'])
#  else
#   curses = dependency('ncursesw')
#   sdl2 = dependency('sdl2')
#   sdl2_ttf = meson.get_compiler('c').find_library('SDL2_ttf')
#  endif
#
#  glfw = dependency('glfw3')
#  math = compiler.find_library('m')
#  dl = compiler.find_library('dl')
#  glad = include_directories('frontend/opengl/glad/include')
#  executable('cetris_gl', [src, 'frontend/opengl/main.c', 'frontend/opengl/glad/src/glad.c'], 
#	       dependencies: [glfw, math, dl],
#         include_directories: [inc, glad],
#         install : false)
#
#  #executable('cetris_curses', [src, 'frontend/ncurses/ncurses_ui.c'], 
#	#       dependencies: curses,
#  #       include_directories: [inc, ,
#  #       install : false)
#  executable('cetris_sdl', [src, 'frontend/sdl/sdl_ui.c'], 
#         include_directories: [inc, sdlinc],
#	       dependencies: [sdl2, sdl2_ttf],
#         install : false)
#endif
