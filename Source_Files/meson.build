SDL = dependency('SDL2')
SDL_image = dependency('SDL2_image')
SDL_ttf = dependency('SDL2_ttf')

Sources = ['Main.cpp', 'Mino/Mino.cpp', 'World/World.cpp', '../SDL-Helper-Libraries/File/File.cpp', '../SDL-Helper-Libraries/Image/Image.cpp']

SourceInclude = include_directories('./')

executable('Tetris', Sources, install : true, dependencies : [SDL, SDL_image, SDL_ttf], include_directories : SourceInclude, install_rpath: prefix + '/bin/')
install_subdir('../res/Data', install_dir : 'bin/')
install_subdir('../res/Minos', install_dir : 'bin/')
install_data('../res/Deadline.png', install_dir : 'bin/')
install_data('../res/DejaVuSans.ttf', install_dir : 'bin/')
install_data('../res/Title.png', install_dir : 'bin/')
install_data('../res/BackGround.png', install_dir : 'bin/')
install_data('../res/5_5_Outline.png', install_dir : 'bin/')
install_data('../res/Surrounding_Line.png', install_dir : 'bin/')
install_data('../res/LevelDecideArrows.png', install_dir : 'bin/')
install_data('../res/SetOptOutline.png', install_dir : 'bin/')
install_data('../res/IndevidualBox.png', install_dir : 'bin/')
