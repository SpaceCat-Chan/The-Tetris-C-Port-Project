project('The-Tetris-C-Port-Project', 'cpp')

SDL = dependency('SDL2')
SDL_image = dependency('SDL2_image')
SDL_ttf = dependency('SDL2_ttf')

BuildDir = meson.current_build_dir()

Sources = ['Source_Files/Main.cpp', 'Source_Files/Mino/Mino.cpp', 'Source_Files/World/World.cpp', 'SDL-Helper-Libraries/File/File.cpp', 'SDL-Helper-Libraries/Image/Image.cpp']

SourceInclude = include_directories('Source_Files/')

executable('Tetris', Sources, install : true, install_dir : BuildDir + '/Install/', dependencies : [SDL, SDL_image, SDL_ttf], include_directories : SourceInclude)
install_subdir('res/Data', install_dir : BuildDir + '/Install/')
install_subdir('res/Minos', install_dir : BuildDir + '/Install/')
install_data('res/Deadline.png', install_dir : BuildDir + '/Install/')
install_data('res/DejaVuSans.ttf', install_dir : BuildDir + '/Install/')
install_data('res/Title.png', install_dir : BuildDir + '/Install/')
install_data('res/BackGround.png', install_dir : BuildDir + '/Install/')
install_data('res/5_5_Outline.png', install_dir : BuildDir + '/Install/')
install_data('res/Surrounding_Line.png', install_dir : BuildDir + '/Install/')

if host_machine.system == 'windows'
	install_data('winres/SDL2.dll', install_dir : BuildDir + '/Install/')
	install_data('winres/zlib1.dll', install_dir : BuildDir + '/Install/')
	install_data('winres/SDL2_ttf.dll', install_dir : BuildDir + '/Install/')
	install_data('winres/SDL2_image.dll', install_dir : BuildDir + '/Install/')
	install_data('winres/libpng16-16.dll', install_dir : BuildDir + '/Install/')
	install_data('winres/libfreetype-6.dll', install_dir : BuildDir + '/Install/')
endif