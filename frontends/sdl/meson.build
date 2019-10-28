src = ['sdl_ui.c']

inc = [cetris_inc]

threads = dependency('threads')
math = compiler.find_library('m')
if host_machine.system() == 'windows'
  sdl = compiler.find_library('SDL2', dirs: meson.current_source_dir() + '/win/SDL2-2.0.10/lib/x64')
  ttf = compiler.find_library('SDL2_ttf', dirs: meson.current_source_dir() + '/win/SDL2_ttf-2.0.15/lib/x64')
  inc += include_directories('win/SDL2-2.0.10/include')
  inc += include_directories('win/SDL2_ttf-2.0.15/include')
else
  sdl = dependency('SDl2')
  ttf = dependency('SDL2_ttf')
endif

configure_file(input: 'config.ini', 
        output: 'config.ini',
        copy: true, install: false)

configure_file(input: 'OpenSans-Regular.tff', 
        output: 'OpenSans-Regular.ttf',
        copy: true, install: false)

executable('cetris', src,
    dependencies: [threads, sdl, ttf, math],
    include_directories: inc,
    install: false)
