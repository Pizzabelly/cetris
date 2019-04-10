src = ['main.c', 'glad/src/glad.c']
glad_inc = include_directories('glad/include')

glfw = dependency('glfw3')
math = compiler.find_library('m')
dl = compiler.find_library('dl')

configure_file(input: 'block.jpg', 
        output: 'block.jpg',
        copy: true, install: false)

executable('cetris_gl', src,
        dependencies: [glfw, math, dl, cetris],
        include_directories: [cetris_inc, glad_inc],
        install: false)
