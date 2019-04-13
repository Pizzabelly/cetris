src = ['curses_ui.c']

inc = []
if host_machine.system() == 'windows'
  inc += include_directories('win')
  curses = compiler.find_library('pdcurses', dirs: meson.current_source_dir() + '/win')
else
  curses = dependency('ncursesw')
endif

executable('cetris_curses', src,
           dependencies: [curses, cetris],
           include_directories: [inc, cetris_inc],
           install: false)
