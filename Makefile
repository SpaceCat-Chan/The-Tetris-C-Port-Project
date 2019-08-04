#OBJS specifies which files to compile as part of the project
OBJS = Source_Files/Main.cpp Source_Files/World/World.cpp Source_Files/Mino/Mino.cpp Source_Files/Image/Image.cpp Source_Files/KeyHandlers/KeyHandlers.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -g -ISource_Files/ -m64
EXTRA_WINDOWS_FLAG = -IMinGW_SDL_Dev/include/

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2main -lSDL2 -lSDL2_image

#Library paths, only needed for windows
Library_Path = -LMinGW_SDL_Dev/lib/

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = Linux_Build/Tetris
WINDOWS_NAME = Windows_Build/Tetris

#This is the target that compiles our executable
linux : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

windows : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(EXTRA_WINDOWS_FLAG) $(Library_Path) $(LINKER_FLAGS) -o $(WINDOWS_NAME)
