src = ['main.c', 'glad/src/gl.c']

inc = [cetris_inc, include_directories('glad/include')]
deps = []

if host_machine.system() == 'windows'
  deps += compiler.find_library('glfw3', dirs: meson.current_source_dir() + '/win/glfw')
  inc += include_directories('win/glfw/include')
else
  deps += dependency('glfw3')
  deps += compiler.find_library('m')
  deps += compiler.find_library('dl')
endif

configure_file(input: 'block.jpg', 
        output: 'block.jpg',
        copy: true, install: false)

executable('cetris', src,
        dependencies: deps,
        include_directories: inc,
        install: false)
