src = ['cetris.c', 'rules.c']
src += is_windows ? 'timer_windows.c' : 'timer_linux.c'

threads = dependency('threads')

cetris_lib = static_library('cetris', src, dependencies: threads,
   include_directories: cetris_inc, install: false)

cetris = declare_dependency(include_directories: cetris_inc,
   link_with: cetris_lib)
