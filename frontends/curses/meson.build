src = ['curses_ui.c']

inc = [cetris_inc]
deps = [cetris]

if host_machine.system() == 'windows'
  inc += include_directories('win')
  deps += compiler.find_library('pdcurses', dirs: meson.current_source_dir() + '/win')
else
  deps += dependency('ncursesw')
  deps += dependency('tinfow')
  deps += dependency('threads')
endif

executable('cetris', src,
           dependencies: deps,
           include_directories: inc,
           install: false)
