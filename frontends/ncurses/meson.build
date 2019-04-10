src = ['ncurses_ui.c']

ncurses = dependency('ncursesw')

executable('cetris_curses', src,
           dependencies: [ncurses, cetris],
           include_directories: cetris_inc,
           install: false)
