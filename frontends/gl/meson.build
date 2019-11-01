src = ['audio.c', 'ui.c', 'skin.c', 'drawable.c', 'shader.c', 'main.c', 'glad/src/glad.c']

inc = [cetris_inc, include_directories('glad/include')]
deps = [cetris]

if host_machine.system() == 'windows'
  deps += compiler.find_library('SDL2', dirs: meson.current_source_dir() + '/win/SDL2-2.0.10/lib/x64')
  deps += compiler.find_library('SDL2_ttf', dirs: meson.current_source_dir() + '/win/SDL2_ttf-2.0.15/lib/x64')
  deps += compiler.find_library('SDL2_image', dirs: meson.current_source_dir() + '/win/SDL2_image-2.0.5/lib/x64')
  inc += include_directories('win/SDL2-2.0.10/include')
  inc += include_directories('win/SDL2_ttf-2.0.15/include')
  inc += include_directories('win/SDL2_image-2.0.5/include')
else
  deps += compiler.find_library('m')
  deps += compiler.find_library('dl')
  deps += dependency('SDl2')
  deps += dependency('SDL2_ttf')
  deps += dependency('SDL2_image')
endif

executable('cetris', src,
  dependencies: deps,
  include_directories: inc,
  install: false)
