#include "File.hpp"

File::File() {

}

File::~File() {
	if(CurrentFile) {
		SDL_RWclose(CurrentFile);
		CurrentFile = nullptr;
	}
}

bool File::OpenFile(std::string FileName, int Mode, void *StanderdInfo=nullptr, int AmountOfInfo=0, int InfoSize=0) {
	std::string OpenMode;
	if(Mode & FileModes::Read) {
		OpenMode += 'r';
	}
	else if(Mode & FileModes::Write) {
		OpenMode += 'w';
	}
	else {
		OpenMode += 'a';
	}

	if(Mode & FileModes::Binary) {
		OpenMode += 'b';
	}
	if(Mode & FileModes::OverLoad) {
		OpenMode += '+';
	}

	CurrentFile = SDL_RWFromFile(FileName.c_str(), OpenMode.c_str());
	if(CurrentFile == nullptr) {
		if(AmountOfInfo == 0 && !(Mode & (FileModes::Write | FileModes::Append))) {
			CurrentError = SDL_GetError();
			return false;
		}

		std::string ErrorMode="w";
		if(Mode & FileModes::Binary) {
			ErrorMode += 'b';
		}
		ErrorMode += '+';

		CurrentFile = SDL_RWFromFile(FileName.c_str(), ErrorMode.c_str());
		if(CurrentFile == nullptr) {
			CurrentError = SDL_GetError();
			return false;
		}

		int AmountWritten=0;
		AmountWritten = SDL_RWwrite(CurrentFile, StanderdInfo, InfoSize , AmountOfInfo);
		if(AmountWritten < AmountOfInfo) {
			SDL_RWclose(CurrentFile);
			CurrentFile = nullptr;
			CurrentError = SDL_GetError();
			return false;
		}
		if(SDL_RWclose(CurrentFile) < 0) {
			CurrentFile = nullptr;
			CurrentError = SDL_GetError();
			return false;
		}
		CurrentFile = SDL_RWFromFile(FileName.c_str(), OpenMode.c_str());
		if(CurrentFile == nullptr) {
			CurrentFile = nullptr;
			CurrentError = SDL_GetError();
			return false;
		}
	}
	return true;
}

bool File::CloseFile() {
	if(CurrentFile != nullptr) {
		if(!SDL_RWclose(CurrentFile)) {
			CurrentError = SDL_GetError();
			CurrentFile = nullptr;
			return false;
		}
	}
	return true;
}

