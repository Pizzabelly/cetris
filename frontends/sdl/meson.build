src = ['sdl_ui.c']

threads = dependency('threads')
sdl = dependency('SDl2')
ttf = dependency('SDL2_ttf')

configure_file(input: 'config.ini', 
        output: 'config.ini',
        copy: true, install: false)

executable('cetris', src,
    dependencies: [threads, sdl, ttf],
    include_directories: cetris_inc,
    install: false)
