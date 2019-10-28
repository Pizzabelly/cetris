src = ['sdl_ui.c']

inc = [cetris_inc]

threads = dependency('threads')
deps = []

if host_machine.system() == 'windows'
  deps += compiler.find_library('SDL2', dirs: meson.current_source_dir() + '/win/SDL2-2.0.10/lib/x64')
  deps += compiler.find_library('SDL2_ttf', dirs: meson.current_source_dir() + '/win/SDL2_ttf-2.0.15/lib/x64')
  inc += include_directories('win/SDL2-2.0.10/include')
  inc += include_directories('win/SDL2_ttf-2.0.15/include')
else
  deps += compiler.find_library('m')
  deps += dependency('SDl2')
  deps += dependency('SDL2_ttf')
endif

configure_file(input: 'config.ini', 
        output: 'config.ini',
        copy: true, install: false)

configure_file(input: 'OpenSans-Regular.ttf', 
        output: 'OpenSans-Regular.ttf',
        copy: true, install: false)

executable('cetris', src,
    dependencies: deps,
    include_directories: inc,
    install: false)
