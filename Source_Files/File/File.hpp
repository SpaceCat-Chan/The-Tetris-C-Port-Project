#include <SDL2/SDL.h>
#include <SDL2/SDL_filesystem.h>

#include <string>

namespace FileModes{
	enum wawawa {
		Read=1,
		Write=2,
		Append=4,
		Binary=8,
		OverLoad=16,
	};
}

class File {
	SDL_RWops *CurrentFile=nullptr;
	std::string CurrentError="";
	public:
	
	File();

	~File();

	bool OpenFile(std::string FileName, int Mode, void *StanderdInfo=nullptr, int AmountOfInfo=0, int InfoSize=0);

	bool CloseFile();

	bool MoveLocation();

	bool Read(void* ReadTo, int Size, int Amount);

	bool Write(void* WriteFrom, int Size, int Amount);
};