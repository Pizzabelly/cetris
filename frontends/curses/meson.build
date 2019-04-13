src = ['curses_ui.c']

inc = []
deps = []
if host_machine.system() == 'windows'
  inc += include_directories('win')
  deps += compiler.find_library('pdcurses', dirs: meson.current_source_dir() + '/win')
else
  deps += dependency('ncursesw')
  deps += dependency('threads')
endif

executable('cetris_curses', src,
           dependencies: [deps, cetris],
           include_directories: [inc, cetris_inc],
           install: false)
