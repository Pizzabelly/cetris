src = ['sdl_ui.c']

deps = [cetris]
inc = [cetris_inc]

if host_machine.system() == 'windows'
  deps += compiler.find_library('SDL2', dirs: meson.current_source_dir() + '/win/SDL2-2.0.10/lib/x64')
  deps += compiler.find_library('SDL2_ttf', dirs: meson.current_source_dir() + '/win/SDL2_ttf-2.0.15/lib/x64')
  inc += include_directories('win/SDL2-2.0.10/include')
  inc += include_directories('win/SDL2_ttf-2.0.15/include')
else
  deps += compiler.find_library('m')
  deps += dependency('SDl2')
  deps += dependency('SDL2_ttf')
  deps += dependency('SDL2_image')
endif

if host_machine.system() != 'windows'
run_command(
  'bash', '-c', 'cp -r $MESON_SOURCE_ROOT/frontends/sdl/data $MESON_BUILD_ROOT/frontends/sdl'
)
endif

executable('cetris', src,
   dependencies: deps,
   include_directories: inc,
   install: false)
