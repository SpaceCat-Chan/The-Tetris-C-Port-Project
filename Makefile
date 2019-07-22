#OBJS specifies which files to compile as part of the project
OBJS = Source_Files/Main.cpp Source_Files/World/World.cpp Source_Files/Mino/Mino.cpp Source_Files/Image/Image.cpp Source_Files/KeyHandlers/KeyHandlers.cpp

#CC specifies which compiler we're using
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using
COMPILER_FLAGS = -g -I/home/spacecatchan/Desktop/Git/The-Tetris-C-Port-Project/Source_Files #-fno-stack-protector

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lSDL2_image

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = Build/Tetris

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
