src = ['src/cetris.c', 'src/input.c', 'src/matrix.c']

if get_option('debug') == true
  src += 'src/test.c'
endif

cetris_lib = static_library('cetris', src,
                include_directories: cetris_inc,
                install: false)

cetris = declare_dependency(include_directories: cetris_inc,
                link_with: cetris_lib)
