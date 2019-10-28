src = ['cetris.c', 'timer.c', 'rules.c']

threads = dependency('threads')

cetris_lib = static_library('cetris', src,
   dependencies: threads,
   include_directories: cetris_inc,
   install: false)

cetris = declare_dependency(include_directories: cetris_inc,
   link_with: cetris_lib)
