project('vara', 'c',
  version: '1.0.0',
  default_options: ['c_std=c11', 'warning_level=1']
)

# Dependencies
gtk_dep = dependency('gtk4')
vte_dep = dependency('vte-2.91-gtk4')

# Include directories
inc_dir = include_directories('include')

# Compile GResources automatically
gnome = import('gnome')
resources = gnome.compile_resources(
  'vara-resources',
  'data/vara.gresource.xml',
  source_dir: 'data'
)

# Source files
sources = [
  'src/main.c',
  'src/callback.c',
  'src/terminal.c',
  'src/window.c',
  resources
]

# Executable target
executable('vara',
  sources,
  include_directories: inc_dir,
  dependencies: [gtk_dep, vte_dep],
  install: true
)
