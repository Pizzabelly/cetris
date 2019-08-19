src = ['cetris.c']

if get_option('debug') == true
  src += 'test.c'
endif

cetris_lib = static_library('cetris', src,
                include_directories: cetris_inc,
                install: false)

cetris = declare_dependency(include_directories: cetris_inc,
                link_with: cetris_lib)
