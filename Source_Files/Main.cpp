/*
TODO:
Add HoldSpot
Add Soft Drop
Add Ghost Piece
Add Upcoming Spot
Add Basic UI
Add Death State
Add Death Screen
Add Pause Screen
Add Score Counting functionality
Add Levels
Add Level Selection
Add Controls and Settings
Add Mode Selection
Add Basic Modes (Hidden)
Add Swap Mode
Add Puyo Mode

Add Finishing Touches

Ideas:
Basic Names for Scores
Online Scoreboard(?)
Stickman Mode
*/

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_image.h>
#include <random>
#include <memory>
#include <math.h>
#include <time.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 720
#define yeet return 1

#define GAME_X 230
#define Text_x 20

/*in lua version there was a thing called ProtSettings
moving into definitions now*/
#define Single 100
#define Double 300
#define Triple 1200
#define Tetris 3200
#define STetris 7000
#define Spin_Multiplier 4


#define Puyo_Types 5
#define ChainLength 7
#define Max_Types_In_One_Piece 2
#define Time_Beetween_action 200
//ProtSettings Finished

#define AmountOfControls 8
#define AmountOfSettings 3

enum TetrominoList {
	S,
	Z,
	L,
	J,
	I,
	T,
	O,
	TOTAL_AMOUNT
};

enum States {
	Menu,
	Game,
	Pause,
	Settings,
	Controls,
};

enum Modes {
	Standard,
	Hidden,
	Swap,
	Puyo,
	TOTAL_MODES
};

enum Rotations {
	ORIGINAL,
	RIGHT,
	DOUBLE,
	LEFT,
	TOTAL_ROTATIONS
};

namespace Buttons {
	const int RotLeft=0,
	RotRight=1,
	HardDR=2,
	SoftDR=3,
	MovLeft=4,
	MovRight=5,
	HoldSpot=6,
	Swap=7,
	Return=8,
	Escape=9;
}



#include "Mino/Mino.hpp"

#include "Image/Image.hpp"

#include "World/World.hpp"

#include "KeyHandlers/KeyHandlers.hpp"


bool init(SDL_Window** window, SDL_Renderer** Render);

void close(SDL_Window* window, SDL_Renderer* Render);

int main(int argc, char* arg[]) {
	SDL_Window* Window=nullptr;
	SDL_Renderer* Render=nullptr;

	if(init(&Window, &Render)) {
		bool Quit=false;

		SDL_Event Event_Handler;

		std::unique_ptr<Image[]> TetrominoImages;
		TetrominoImages = std::make_unique<Image[]>(TetrominoList::TOTAL_AMOUNT);

		std::cout << "Loading Main Images\n";

		TetrominoImages[TetrominoList::S].LoadImage("Minos/SPiece.png", Render);
		TetrominoImages[TetrominoList::Z].LoadImage("Minos/ZPiece.png", Render);
		TetrominoImages[TetrominoList::L].LoadImage("Minos/LPiece.png", Render);
		TetrominoImages[TetrominoList::J].LoadImage("Minos/JPiece.png", Render);
		TetrominoImages[TetrominoList::I].LoadImage("Minos/IPiece.png", Render);
		TetrominoImages[TetrominoList::T].LoadImage("Minos/TPiece.png", Render);
		TetrominoImages[TetrominoList::O].LoadImage("Minos/OPiece.png", Render);

		std::unique_ptr<Image[]> GhostImages;
		GhostImages = std::make_unique<Image[]>(TetrominoList::TOTAL_AMOUNT);

		std::cout << "Loading Ghost Images\n";

		GhostImages[TetrominoList::S].LoadImage("Minos/SPieceGhost.png", Render);
		GhostImages[TetrominoList::Z].LoadImage("Minos/ZPieceGhost.png", Render);
		GhostImages[TetrominoList::L].LoadImage("Minos/LPieceGhost.png", Render);
		GhostImages[TetrominoList::J].LoadImage("Minos/LPieceGhost.png", Render);
		GhostImages[TetrominoList::I].LoadImage("Minos/IPieceGhost.png", Render);
		GhostImages[TetrominoList::T].LoadImage("Minos/TPieceGhost.png", Render);
		GhostImages[TetrominoList::O].LoadImage("Minos/OPieceGhost.png", Render);

		std::cout << "Loading other images\n";

		Image Blank;
		Blank.LoadImage("Minos/Blank.png", Render);

		Image Background, Title, SurroundingLine, DeadLine;
		Background.LoadImage("BackGround.png", Render);
		Title.LoadImage("Title.png", Render);
		SurroundingLine.LoadImage("Surrounding_Line.png", Render);
		DeadLine.LoadImage("Deadline.png", Render);

		Background.SetColor(128, 128, 128);

		std::cout << "finished loading images\n";

		World MainWorld, EmptyWorld;
		Tetromino MainSet, HoldSet;
		Tetromino *MainTetromino, *HoldTetromino;

		MainTetromino = &MainSet;
		HoldTetromino = &HoldSet;

		MainWorld.SetImages(&TetrominoImages, &Blank);
		EmptyWorld.SetImages(&TetrominoImages, &Blank);

		std::mt19937 Engine;
		Engine.seed(time(nullptr));

		MainTetromino->ResetShape(Engine);
		MainTetromino->SetLocation(5, 22);
		MainTetromino->SetImages(&TetrominoImages);

		bool HoldState;
		HoldState = false;

		int CurrentMode = Modes::Standard;
		int CurrentState = States::Menu;

		int RotateLeft = SDLK_z;
		int RotateRight = SDLK_c;
		int HardDrop = SDLK_x;
		int Hold = SDLK_UP;
		int MoveLeft = SDLK_LEFT;
		int MoveRight = SDLK_RIGHT;
		int SoftDrop = SDLK_DOWN;


		long StandardHighscoresTable[5];
		SDL_RWops* StandardHighscores = nullptr;
		StandardHighscores = SDL_RWFromFile("Data/StandardHighscores.bin", "rb");
		if(!StandardHighscores) {
			std::cout << "Warning, StandardHighscores not found, Creating File SDL_ERROR" << SDL_GetError() << '\n';
			StandardHighscores = SDL_RWFromFile("Data/StandardHighscores.bin", "wb");
			if(!StandardHighscores) {
				std::cout << "Unable to create File, SDL_ERROR: " << SDL_GetError() << "\n";
				Quit = true;
				SDL_Delay(5000);
			}
			if(!Quit) {
				int Temp=0;
				for(int i=0; i<5; i++) {
					size_t Size;
					Size = sizeof(long);
					if(SDL_RWwrite(StandardHighscores, &Temp, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
				}
				SDL_RWclose(StandardHighscores);
				SDL_RWFromFile("Data/StandardHighscores.bin", "rb");
			}
		}
		uint8_t ControlsTable[AmountOfControls];
		SDL_RWops* ControlsFile = nullptr;
		if(!Quit) {
			SDL_RWread(StandardHighscores, StandardHighscoresTable, sizeof(long), 5);
			SDL_RWclose(StandardHighscores);

			ControlsFile = SDL_RWFromFile("Data/Controls.bin", "rb");
			if(!ControlsFile) {
				std::cout << "Warning, StandardHighscores not found, Creating File SDL_ERROR" << SDL_GetError() << '\n';
				ControlsFile = SDL_RWFromFile("Data/Controls.bin", "wb");
				if(!ControlsFile) {
					std::cout << "Unable to create File, SDL_ERROR: " << SDL_GetError() << "\n";
					Quit = true;
					SDL_Delay(5000);
				}
				if(!Quit) {
					int RotLeft=SDL_SCANCODE_Z;
					int RotRight=SDL_SCANCODE_C;
					int HardDR=SDL_SCANCODE_X;
					int SoftDR=SDL_SCANCODE_DOWN;
					int MovLeft=SDL_SCANCODE_LEFT;
					int MovRight=SDL_SCANCODE_RIGHT;
					int HoldSpot=SDL_SCANCODE_UP;
					int SwapButton=SDL_SCANCODE_S;
					size_t Size;
					Size = sizeof(uint8_t);
					if(SDL_RWwrite(ControlsFile, &RotLeft, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &RotRight, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &HardDR, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &SoftDR, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &MovLeft, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &MovRight, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &HoldSpot, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &SwapButton, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					
					SDL_RWclose(ControlsFile);
					SDL_RWFromFile("Data/Controls.bin", "rb");
				}
			}
		}

		long SettingsTable[AmountOfSettings];
		SDL_RWops* SettingsFile;
		if(!Quit) {
			SDL_RWread(ControlsFile, ControlsTable, sizeof(uint8_t), 8);
			SDL_RWclose(ControlsFile);
			
			SettingsFile = SDL_RWFromFile("Data/Settings.bin", "rb");
			if(!SettingsFile) {
				std::cout << "Warning, StandardHighscores not found, Creating File SDL_ERROR" << SDL_GetError() << '\n';
				SettingsFile = SDL_RWFromFile("Data/Settings.bin", "wb");
				if(!SettingsFile) {
					std::cout << "Unable to create File, SDL_ERROR: " << SDL_GetError() << "\n";
					Quit = true;
					SDL_Delay(5000);
				}
				if(!Quit) {
					int AutoRepeat_Delay=170;
					int AutoRepeat_Speed=50;
					int PentominoSetting=0;
					
					size_t Size;
					Size = sizeof(long);
					if(SDL_RWwrite(SettingsFile, &AutoRepeat_Delay, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(SettingsFile, &AutoRepeat_Speed, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(SettingsFile, &PentominoSetting, Size, 1) != 1) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					
					SDL_RWclose(SettingsFile);
					SDL_RWFromFile("Data/Settings.bin", "rb");
				}
			}
		}

		if(!Quit) {
			SDL_RWread(SettingsFile, SettingsTable, sizeof(long), AmountOfSettings);
			SDL_RWclose(SettingsFile);
		}

		KeyHandler KeyHandlerList[AmountOfControls + 2];

		KeyHandlerList[Buttons::MovLeft].SetValues(SettingsTable[1], SettingsTable[0], true);
		KeyHandlerList[Buttons::MovRight].SetValues(SettingsTable[1], SettingsTable[0], true);
		KeyHandlerList[Buttons::SoftDR].SetValues(SettingsTable[1], SettingsTable[0], true);

		unsigned long LastTime;
		LastTime = SDL_GetTicks();

		unsigned long Level=1, MoveDownTimer=0, TotalLines=0, TotalScore=0;

		while(!Quit) {
			while(SDL_PollEvent(&Event_Handler)) {
				if(Event_Handler.type == SDL_QUIT) {
					Quit = true;
				}
			}
			const Uint8* PressedKeys;
			PressedKeys = SDL_GetKeyboardState(NULL);
			
			PressedKeys[ControlsTable[Buttons::MovLeft]] ? KeyHandlerList[Buttons::MovLeft].Press() : KeyHandlerList[Buttons::MovLeft].UnPress();
			PressedKeys[ControlsTable[Buttons::MovRight]] ? KeyHandlerList[Buttons::MovRight].Press() : KeyHandlerList[Buttons::MovRight].UnPress();
			PressedKeys[ControlsTable[Buttons::SoftDR]] ? KeyHandlerList[Buttons::SoftDR].Press() : KeyHandlerList[Buttons::SoftDR].UnPress();
			PressedKeys[ControlsTable[Buttons::HardDR]] ? KeyHandlerList[Buttons::HardDR].Press() : KeyHandlerList[Buttons::HardDR].UnPress();
			PressedKeys[ControlsTable[Buttons::HoldSpot]] ? KeyHandlerList[Buttons::HoldSpot].Press() : KeyHandlerList[Buttons::HoldSpot].UnPress();
			PressedKeys[ControlsTable[Buttons::RotLeft]] ? KeyHandlerList[Buttons::RotLeft].Press() : KeyHandlerList[Buttons::RotLeft].UnPress();
			PressedKeys[ControlsTable[Buttons::RotRight]] ? KeyHandlerList[Buttons::RotRight].Press() : KeyHandlerList[Buttons::RotRight].UnPress();
			PressedKeys[ControlsTable[Buttons::Swap]] ? KeyHandlerList[Buttons::Swap].Press() : KeyHandlerList[Buttons::Swap].UnPress();
			PressedKeys[SDL_SCANCODE_RETURN] ? KeyHandlerList[Buttons::Return].Press() : KeyHandlerList[Buttons::Return].UnPress();
			PressedKeys[SDL_SCANCODE_ESCAPE] ? KeyHandlerList[Buttons::Escape].Press() : KeyHandlerList[Buttons::Escape].UnPress();

			unsigned long Time, Temp;
			Temp = SDL_GetTicks();
			Time = Temp - LastTime;
			LastTime = Temp;

			Uint8 KeyStates[AmountOfControls + 2];

			KeyStates[Buttons::MovLeft] = KeyHandlerList[Buttons::MovLeft].Tick(Time);
			KeyStates[Buttons::MovRight] = KeyHandlerList[Buttons::MovRight].Tick(Time);
			KeyStates[Buttons::SoftDR] = KeyHandlerList[Buttons::SoftDR].Tick(Time);
			KeyStates[Buttons::HardDR] = KeyHandlerList[Buttons::HardDR].Tick(Time);
			KeyStates[Buttons::HoldSpot] = KeyHandlerList[Buttons::HoldSpot].Tick(Time);
			KeyStates[Buttons::RotLeft] = KeyHandlerList[Buttons::RotLeft].Tick(Time);
			KeyStates[Buttons::RotRight] = KeyHandlerList[Buttons::RotRight].Tick(Time);
			KeyStates[Buttons::Swap] = KeyHandlerList[Buttons::Swap].Tick(Time);
			KeyStates[Buttons::Return] = KeyHandlerList[Buttons::Return].Tick(Time);
			KeyStates[Buttons::Escape] = KeyHandlerList[Buttons::Escape].Tick(Time);

			if(CurrentState == States::Game) {
				if(MoveDownTimer > ((85.52 * pow(0.88, Level)) * 10)) {
					if(MainTetromino->MoveDown(&MainWorld)) {
						std::unique_ptr<int[]> Check;
						int Amount;
						Amount = MainWorld.CheckLines(Check);
						for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
							MainWorld.ClearLine((Check)[i]);
						}
						if(SettingsTable[2 /* 2 = Pentomino Setting */] == 0 /* 0=false */) {
							MainTetromino->ResetShape(Engine, 3, CurrentMode);
							MainTetromino->SetLocation(5, 22);
							MainTetromino->SetImages(&TetrominoImages);
						}
						else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
							std::uniform_int_distribution<int> SizeSelect(3,5);
							std::mt19937 Engine;
							int SelectedLength = (floor((SizeSelect(Engine) - 3) / 2) + 3);
							MainTetromino->ResetShape(Engine, SelectedLength, CurrentMode);
							MainTetromino->SetLocation(5, 22);
							MainTetromino->SetImages(&TetrominoImages);
						}
						else if(SettingsTable[2] == 2 /* 2=true */) {
							MainTetromino->ResetShape(Engine, 4, CurrentMode);
							MainTetromino->SetLocation(5, 22);
							MainTetromino->SetImages(&TetrominoImages);
						}
						MainWorld.SetImages(&TetrominoImages, &Blank);
						HoldState = false;
					}
					MoveDownTimer = Time;
				}
				else {
					MoveDownTimer += Time;
				}
				if(KeyStates[Buttons::MovLeft]) {
					MainTetromino->MoveSide(-1, &MainWorld);
				}
				else if(KeyStates[Buttons::MovRight]) {
					MainTetromino->MoveSide(1, &MainWorld);
				}
				if(KeyStates[Buttons::RotLeft]) {
					MainTetromino->RotateSelf(true, &MainWorld);
				}
				else if(KeyStates[Buttons::RotRight]) {
					MainTetromino->RotateSelf(false, &MainWorld);
				}
				if(KeyStates[Buttons::HardDR]) {
					bool Down;
					do {
						Down = MainTetromino->MoveDown(&MainWorld);
						if(Down) {
							std::unique_ptr<int[]> Check;
							int Amount;
							Amount = MainWorld.CheckLines(Check);
							for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
								MainWorld.ClearLine((Check)[i]);
							}
							if(SettingsTable[2 /* 3 = Pentomino Setting */] == 0 /* 0=false */) {
								MainTetromino->ResetShape(Engine, 3, CurrentMode);
								MainTetromino->SetLocation(5, 22);
								MainTetromino->SetImages(&TetrominoImages);
							}
							else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
								std::uniform_int_distribution<int> SizeSelect(3,5);
								std::mt19937 Engine;
								int SelectedLength = (floor((SizeSelect(Engine) - 3) / 2) + 3);
								MainTetromino->ResetShape(Engine, SelectedLength, CurrentMode);
								MainTetromino->SetLocation(5, 22);
								MainTetromino->SetImages(&TetrominoImages);
							}
							else if(SettingsTable[2] == 2 /* 2=true */) {
								MainTetromino->ResetShape(Engine, 4, CurrentMode);
								MainTetromino->SetLocation(5, 22);
								MainTetromino->SetImages(&TetrominoImages);
							}
							MainWorld.SetImages(&TetrominoImages, &Blank);
						}
					}
					while(!Down);
					HoldState = false;
					MoveDownTimer = 0;
				}
				if(KeyStates[Buttons::HoldSpot]) {
					if(!HoldState) {
						std::swap(HoldTetromino, MainTetromino);
						if(MainTetromino->GetRotation() == -1) {
							MainTetromino->ResetShape(Engine, 4, CurrentMode);
							MainTetromino->SetLocation(5, 22);
							MainTetromino->SetImages(&TetrominoImages);
						}
						HoldTetromino->SetLocation(5, 22);
						HoldState = true;
					}
				}
			}
			else if(CurrentState == States::Menu) {
				if(KeyStates[Buttons::Return]) {
					MainWorld.Reset();
					MainWorld.SetImages(&TetrominoImages, &Blank);
					if(SettingsTable[2 /* 3 = Pentomino Setting */] == 0 /* 0=false */) {
						MainTetromino->ResetShape(Engine, 3, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
						std::uniform_int_distribution<int> SizeSelect(3,5);
						std::mt19937 Engine;
						int SelectedLength = (floor((SizeSelect(Engine) - 3) / 2) + 3);
						MainTetromino->ResetShape(Engine, SelectedLength, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					else if(SettingsTable[2] == 2 /* 2=true */) {
						MainTetromino->ResetShape(Engine, 4, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					CurrentState = States::Game;
				}
			}
			
			SDL_RenderClear(Render);

			Background.Draw(0, 0, Render);

			SurroundingLine.Draw(GAME_X-10, SCREEN_HEIGHT - 28*25 - 10, Render);

			if(CurrentState == States::Game) {
				MainWorld.Draw(Render, GAME_X, 0);
				MainTetromino->Draw(Render, GAME_X, 0);
				DeadLine.Draw(GAME_X, 156, Render);
			}
			else {
				EmptyWorld.Draw(Render, GAME_X, 0);
			}

			SDL_RenderPresent(Render);
			SDL_Delay(50);
		}
		close(Window, Render);
	}

	yeet;
}

bool init(SDL_Window** window, SDL_Renderer** Render) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
		return false;
	}
	else {
		*window = SDL_CreateWindow("Fook u", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if(*window == NULL) {
			std::cout << "Window could not be created, SDL_Error: " << SDL_GetError() << '\n';
			return false;
		}
		else {
			int Img_Flags = IMG_INIT_PNG;
			if(!(IMG_Init(Img_Flags) & Img_Flags)) {
				std::cout << "Unable to load PNG loader, IMG_ERROR: " << IMG_GetError() << '\n';
				return false;
			}
			else {
				*Render = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
				if(*Render == NULL) {
					std::cout << "unable to create Renderer, SDL_ERROR" << SDL_GetError() << '\n';
					return false;
				}
				return true;
			}
		}
	}
}

void close(SDL_Window* window, SDL_Renderer* Render) {
	SDL_DestroyWindow( window );
	SDL_DestroyRenderer(Render);

	IMG_Quit();

	SDL_Quit();

	std::cout << "exited succesfully\n";
}
