project('The-Tetris-C-Port-Project', 'cpp')

projectDir = meson.current_source_dir()
BuildDir = meson.build_root()

GNSlibsPath = join_paths(projectDir, 'libs/GameNetworkingSockets/libs')

cpp_compiler = meson.get_compiler('cpp')

prefix = get_option('prefix')
prefix = '/'.join(prefix.split('\\'))

subdir('Source_Files/')
