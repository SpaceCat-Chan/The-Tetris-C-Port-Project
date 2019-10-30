/*
TODO:
Add Basic UI
	Add Level Selection

Add Pause Screen
Add Controls and Settings

Add Mode Selection
	Add Basic Modes (Hidden)
	Add Swap Mode
	Add Puyo Mode

Add Finishing Touches

Ideas:
Online Scoreboard(?)
Stickman Mode
*/

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <random>
#include <memory>
#include <math.h>
#include <time.h>
#include <sstream>
#include <algorithm>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 720
#define yeet return 1

#define GAME_X 230
#define Text_X 20

/*in lua version there was a thing called ProtSettings
moving into definitions now*/
#define Single 1
#define Double 3
#define Triple 10
#define Tetris 32
#define STetris 70
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
	Dead
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

#include "../SDL-Helper-Libraries/Image/Image.hpp"

#include "World/World.hpp"

#include "../SDL-Helper-Libraries/File/File.hpp"


void WriteNumber(SDL_Renderer *Render, long Number, Image *Numbers, int x, int y, TTF_Font *font) {
	std::stringstream ToString;
	ToString << Number;
	unsigned long Distance=0;
	for(auto& Char : ToString.str()) {
		if(Char == '\00') {
			continue;
		}
		int TempNumber;
		std::stringstream Temp;
		Temp << Char;
		Temp >> TempNumber;
		Numbers[TempNumber].Draw(Distance + x, y, Render);
		char TempChar[2] = {Char, '\00'};
		int w,h;
		TTF_SizeUTF8(font, TempChar, &w, &h);
		Distance += w;
	}
}


bool init(SDL_Window** window, SDL_Renderer** Render);

void close(SDL_Window* window, SDL_Renderer* Render);


bool ClearLines(SDL_Renderer *Render, Tetromino *MainTetromino, World *MainWorld, unsigned long &TotalLines, unsigned long &TotalScore, const long Level, unsigned long *ScoreList, long *SettingsTable, Image* LastClearImage, std::string *LineTypes, TTF_Font *BigFont, std::unique_ptr<Image[]> *TetrominoImages, Image &Blank, int &CurrentMode, std::mt19937 &Engine, Tetromino &UpcommingTetromino, bool &HoldState) {
	if(MainTetromino->MoveDown(MainWorld)) {
		std::unique_ptr<int[]> Check;
		int Amount;
		Amount = MainWorld->CheckLines(Check);
		for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
			MainWorld->ClearLine((Check)[i]);
		}

		//TODO: add t-spin detection

		bool IsTSpin=false;
		TotalLines += (Amount);
		if(Amount > 0) {
			TotalScore += ScoreList[Amount-1] * (Level+1) * (IsTSpin ? Spin_Multiplier : 1);
		}

		if(Amount != 0) {
			LastClearImage->LoadFromText(LineTypes[Amount - 1 + IsTSpin*5], BigFont, Render, {255, 255, 255, 255});
		}
		else {
			LastClearImage->LoadFromText(" ", BigFont, Render, {255, 255, 255, 255});
		}
		if(SettingsTable[2 /* 2 = Pentomino Setting */] == 0 /* 0=false */) {
			MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 3, CurrentMode);
			MainTetromino->SetLocation(5, 22);
			MainTetromino->SetImages(TetrominoImages);
		}
		else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
			std::uniform_int_distribution<int> SizeSelect(3,5);
			std::mt19937 Engine;
			int SelectedLength = (floor((SizeSelect(Engine) - 3) / 2) + 3);
			MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, SelectedLength, CurrentMode);
			MainTetromino->SetLocation(5, 22);
			MainTetromino->SetImages(TetrominoImages);
		}
		else if(SettingsTable[2] == 2 /* 2=true */) {
			MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 4, CurrentMode);
			MainTetromino->SetLocation(5, 22);
			MainTetromino->SetImages(TetrominoImages);
		}
		MainWorld->SetImages(TetrominoImages, &Blank);
		UpcommingTetromino.SetImages(TetrominoImages);
		UpcommingTetromino.SetLocation(2,2);
		HoldState = false;

		return true;
	}
	return false;
}


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
		GhostImages[TetrominoList::J].LoadImage("Minos/JPieceGhost.png", Render);
		GhostImages[TetrominoList::I].LoadImage("Minos/IPieceGhost.png", Render);
		GhostImages[TetrominoList::T].LoadImage("Minos/TPieceGhost.png", Render);
		GhostImages[TetrominoList::O].LoadImage("Minos/OPieceGhost.png", Render);

		std::cout << "Loading other images\n";

		Image Blank;
		Blank.LoadImage("Minos/Blank.png", Render);

		Image Background, Title, SurroundingLine, DeadLine, Outline;
		Background.LoadImage("BackGround.png", Render);
		Title.LoadImage("Title.png", Render);
		SurroundingLine.LoadImage("Surrounding_Line.png", Render);
		DeadLine.LoadImage("Deadline.png", Render);
		Outline.LoadImage("5_5_Outline.png", Render);

		World UpcommingBackground{5,5}, HoldSpotBackground{5,5};
		UpcommingBackground.SetImages(&TetrominoImages, &Blank);
		HoldSpotBackground.SetImages(&TetrominoImages, &Blank);

		Background.SetColor(128, 128, 128);

		std::cout << "finished loading images\n";

		TTF_Font *SmallFont, *BigFont;

		SmallFont = TTF_OpenFont("DejaVuSans.ttf", 18); //deja vu, i have been in this bad time before.
		BigFont = TTF_OpenFont("DejaVuSans.ttf", 36);

		if(!SmallFont || !BigFont) {
			std::cout << "unable to load Fonts, TTF_ERROR: " << TTF_GetError() << '\n';
		}

		Image DeathText;
		DeathText.LoadFromText("You Died", BigFont, Render, {255,255,255});

		Image TotalLinesText[2], Score[2], LevelText;
		TotalLinesText[0].LoadFromText("TotalLines: ", SmallFont, Render, {255, 255, 255, 255});
		Score[0].LoadFromText("Score: ", SmallFont, Render, {255, 255, 255, 255});
		LevelText.LoadFromText("Level: ", SmallFont, Render, {255, 255, 255});


		std::cout << "Finished loading Fonts and Static Text\n";

		World MainWorld, EmptyWorld;
		Tetromino MainSet, HoldSet, Ghost, UpcommingTetromino;
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
		UpcommingTetromino.SetImages(&TetrominoImages);
		HoldTetromino->SetImages(&TetrominoImages);

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


		long StandardHighscoresTableStandard[5] = {0,0,0,0,0};
		long HighscoresTable[6] = {0, 0, 0, 0, 0, 0};

		File StandardHighscoresFile;
		if(!StandardHighscoresFile.OpenFile("Data/StandardHighscores.bin", FileModes::Read | FileModes::Binary, StandardHighscoresTableStandard, 5, long)) {
			SDL_Log("FileError: %s\n", StandardHighscoresFile.GetError().c_str());
		}
		if(!StandardHighscoresFile.Read(HighscoresTable, long, 5)) {
			SDL_Log("FileError: %s\n", StandardHighscoresFile.GetError().c_str());
		}
		if(!StandardHighscoresFile.CloseFile()) {
			SDL_Log("FileError: %s\n", StandardHighscoresFile.GetError().c_str());
		}

		File ControlsFile;
		uint8_t ControlsTable[AmountOfControls];
		uint8_t ControlsTableStandard[AmountOfControls] = {SDL_SCANCODE_Z, SDL_SCANCODE_C, SDL_SCANCODE_X, SDL_SCANCODE_DOWN,
															SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_S};

		if(!ControlsFile.OpenFile("Data/Controls.bin", FileModes::Read | FileModes::Binary, ControlsTableStandard, AmountOfControls, uint8_t)) {
			SDL_Log("FileError: %s\n", ControlsFile.GetError().c_str());
		}

		if(!ControlsFile.Read(ControlsTable, uint8_t, AmountOfControls)) {
			SDL_Log("FileError: %s\n", ControlsFile.GetError().c_str());
		}

		if(!ControlsFile.CloseFile()) {
			SDL_Log("FileError: %s\n", ControlsFile.GetError().c_str());
		}
	

		File SettingsFile;
		long SettingsTableStandard[AmountOfSettings] = {170, 50, 0};
		long SettingsTable[AmountOfSettings];

		if(!SettingsFile.OpenFile("Data/Settings.bin", FileModes::Read | FileModes::Binary, SettingsTableStandard, AmountOfSettings, long)) {
			SDL_Log("FileError: %s\n", ControlsFile.GetError().c_str());
		}

		if(!SettingsFile.Read(SettingsTable, long, AmountOfSettings)) {
			SDL_Log("FileError: %s\n", ControlsFile.GetError().c_str());
		}

		if(!SettingsFile.CloseFile()) {
			SDL_Log("FileError: %s\n", SettingsFile.GetError().c_str());
		}

		unsigned long LastTime;
		LastTime = SDL_GetTicks();

		Image LastClearImage;

		unsigned long Level=1, MoveDownTimer=0, TotalLines=0, TotalScore=0, LastMove=10;
		std::string LineTypes[8] = {"Single","Double","Triple","Tetris!","SUPER\nTETRIS!","T-Spin\nSingle","T-Spin\nDouble!","T-SPIN\nTRIPLE!"};
		LastClearImage.LoadFromText(" ", BigFont, Render, {255, 255, 255, 255});

		unsigned long ScoreList[] = {Single, Double, Triple, Tetris, STetris};

		Image HighScoreTitle, HighscoresText[5];
		HighscoresText[0].LoadFromText("1:", SmallFont, Render, {255, 255, 255, 255});
		HighscoresText[1].LoadFromText("2:", SmallFont, Render, {255, 255, 255, 255});
		HighscoresText[2].LoadFromText("3:", SmallFont, Render, {255, 255, 255, 255});
		HighscoresText[3].LoadFromText("4:", SmallFont, Render, {255, 255, 255, 255});
		HighscoresText[4].LoadFromText("5:", SmallFont, Render, {255, 255, 255, 255});
		HighScoreTitle.LoadFromText("Highscores:", SmallFont, Render, {255, 255, 255, 255});
		
		Image Numbers[10];
		Numbers[0].LoadFromText("0", SmallFont, Render, {255, 255, 255, 255});
		Numbers[1].LoadFromText("1", SmallFont, Render, {255, 255, 255, 255});
		Numbers[2].LoadFromText("2", SmallFont, Render, {255, 255, 255, 255});
		Numbers[3].LoadFromText("3", SmallFont, Render, {255, 255, 255, 255});
		Numbers[4].LoadFromText("4", SmallFont, Render, {255, 255, 255, 255});
		Numbers[5].LoadFromText("5", SmallFont, Render, {255, 255, 255, 255});
		Numbers[6].LoadFromText("6", SmallFont, Render, {255, 255, 255, 255});
		Numbers[7].LoadFromText("7", SmallFont, Render, {255, 255, 255, 255});
		Numbers[8].LoadFromText("8", SmallFont, Render, {255, 255, 255, 255});
		Numbers[9].LoadFromText("9", SmallFont, Render, {255, 255, 255, 255});

		bool PressedKeys[AmountOfControls + 2];
		int PressedFor[AmountOfControls + 2];
		for(int i=0; i<AmountOfControls+2; i++) {
			PressedKeys[i] = false;
			PressedFor[i] = 0;
		}

		while(!Quit) {

			unsigned long Time, Temp;
			Temp = SDL_GetTicks();
			Time = Temp - LastTime;
			LastTime = Temp;

			Uint8 KeyStates[AmountOfControls + 2], AlreadyLetGo[AmountOfControls + 2];
			for(int i=0; i<AmountOfControls+2; i++) {
				KeyStates[i] = false;
				AlreadyLetGo[i] = false;
			}

			while(SDL_PollEvent(&Event_Handler)) {
				if(Event_Handler.type == SDL_QUIT) {
					Quit = true;
				}

				if(Event_Handler.type == SDL_KEYDOWN && !Event_Handler.key.repeat) {
					if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::MovLeft]) {
						if(!AlreadyLetGo[Buttons::MovLeft]) {
							PressedKeys[Buttons::MovLeft] = true;
							PressedFor[Buttons::MovLeft] = 0;
						}
						KeyStates[Buttons::MovLeft] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::MovRight]) {
						if(!AlreadyLetGo[Buttons::MovRight]) {
							PressedKeys[Buttons::MovRight] = true;
							PressedFor[Buttons::MovRight] = 0;
						}
						KeyStates[Buttons::MovRight] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::SoftDR]) {
						if(!AlreadyLetGo[Buttons::SoftDR]) {
							PressedKeys[Buttons::SoftDR] = true;
							PressedFor[Buttons::SoftDR] = 0;
						}
						KeyStates[Buttons::SoftDR] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::HardDR]) {
						if(!AlreadyLetGo[Buttons::HardDR]) {
							PressedKeys[Buttons::HardDR] = true;
							PressedFor[Buttons::HardDR] = 0;
						}
						KeyStates[Buttons::HardDR] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::HoldSpot]) {
						if(!AlreadyLetGo[Buttons::HoldSpot]) {
							PressedKeys[Buttons::HoldSpot] = true;
							PressedFor[Buttons::HoldSpot] = 0;
						}
						KeyStates[Buttons::HoldSpot] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::RotLeft]) {
						if(!AlreadyLetGo[Buttons::RotLeft]) {
							PressedKeys[Buttons::RotLeft] = true;
							PressedFor[Buttons::RotLeft] = 0;
						}
						KeyStates[Buttons::RotLeft] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::RotRight]) {
						if(!AlreadyLetGo[Buttons::RotRight]) {
							PressedKeys[Buttons::RotRight] = true;
							PressedFor[Buttons::RotRight] = 0;
						}
						KeyStates[Buttons::RotRight] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::Swap]) {
						if(!AlreadyLetGo[Buttons::Swap]) {
							PressedKeys[Buttons::Swap] = true;
							PressedFor[Buttons::Swap] = 0;
						}
						KeyStates[Buttons::Swap] = true;
					}
					else if(Event_Handler.key.keysym.scancode == SDL_SCANCODE_RETURN) {
						if(!AlreadyLetGo[Buttons::Return]) {
							PressedKeys[Buttons::Return] = true;
							PressedFor[Buttons::Return] = 0;
						}
						KeyStates[Buttons::Return] = true;
					}
					else if(Event_Handler.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						if(!AlreadyLetGo[Buttons::Escape]) {
							PressedKeys[Buttons::Escape] = true;
							PressedFor[Buttons::Escape] = 0;
						}
						KeyStates[Buttons::Escape] = true;
					}
				}
				if(Event_Handler.type == SDL_KEYUP) {
					if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::MovLeft]) {
						PressedKeys[Buttons::MovLeft] = false;
						AlreadyLetGo[Buttons::MovLeft] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::MovRight]) {
						PressedKeys[Buttons::MovRight] = false;
						AlreadyLetGo[Buttons::MovRight] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::SoftDR]) {
						PressedKeys[Buttons::SoftDR] = false;
						AlreadyLetGo[Buttons::SoftDR] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::HardDR]) {
						PressedKeys[Buttons::HardDR] = false;
						AlreadyLetGo[Buttons::HardDR] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::HoldSpot]) {
						PressedKeys[Buttons::HoldSpot] = false;
						AlreadyLetGo[Buttons::HoldSpot] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::RotLeft]) {
						PressedKeys[Buttons::RotLeft] = false;
						AlreadyLetGo[Buttons::RotLeft] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::RotRight]) {
						PressedKeys[Buttons::RotRight] = false;
						AlreadyLetGo[Buttons::RotRight] = true;
					}
					else if(Event_Handler.key.keysym.scancode == ControlsTable[Buttons::Swap]) {
						PressedKeys[Buttons::Swap] = false;
						AlreadyLetGo[Buttons::Swap] = true;
					}
					else if(Event_Handler.key.keysym.scancode == SDL_SCANCODE_RETURN) {
						PressedKeys[Buttons::Return] = false;
						AlreadyLetGo[Buttons::Return] = true;
					}
					else if(Event_Handler.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						PressedKeys[Buttons::Escape] = false;
						AlreadyLetGo[Buttons::Escape] = true;
					}
				}

			}

			for(int i=0; i<AmountOfControls+2; i++) {
				if(PressedKeys[i]) {
					PressedFor[i] += Time;
				}
			}

			for(int i=0; i < AmountOfControls + 2; i++) {
				if(PressedFor[i] > SettingsTable[0]+SettingsTable[1]) {
					if(i == Buttons::SoftDR || i == Buttons::MovLeft || i == Buttons::MovRight) {
						KeyStates[i] = true;
						PressedFor[i] = SettingsTable[0];
					}
				}
			}
			
			//Logic handeling (timers, moving things, menues, all that stuff)

			Level = TotalLines/10;

			if(CurrentState == States::Game) {
				if(MoveDownTimer > ((85.52 * pow(0.88, Level)) * 10)) {
					if(ClearLines(Render, MainTetromino, &MainWorld, TotalLines, TotalScore, Level, ScoreList, SettingsTable, &LastClearImage, LineTypes, BigFont, &TetrominoImages, Blank, CurrentMode, Engine, UpcommingTetromino, HoldState))
						Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
	
					MoveDownTimer = Time;
				}
				else {
					MoveDownTimer += Time;
				}
				if(KeyStates[Buttons::MovLeft]) {
					MainTetromino->MoveSide(-1, &MainWorld);
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
				}
				else if(KeyStates[Buttons::MovRight]) {
					MainTetromino->MoveSide(1, &MainWorld);
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
				}
				if(KeyStates[Buttons::RotLeft]) {
					MainTetromino->RotateSelf(true, &MainWorld);
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
				}
				else if(KeyStates[Buttons::RotRight]) {
					MainTetromino->RotateSelf(false, &MainWorld);
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
				}
				if(KeyStates[Buttons::HardDR]) {
					bool Down;
					do {
						Down = ClearLines(Render, MainTetromino, &MainWorld, TotalLines, TotalScore, Level, ScoreList, SettingsTable, &LastClearImage, LineTypes, BigFont, &TetrominoImages, Blank, CurrentMode, Engine, UpcommingTetromino, HoldState);
					}
					while(!Down);
					HoldState = false;
					MoveDownTimer = 0;
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
					UpcommingTetromino.SetImages(&TetrominoImages);
					UpcommingTetromino.SetLocation(2,2);
				}
				if(KeyStates[Buttons::SoftDR]) {
					ClearLines(Render, MainTetromino, &MainWorld, TotalLines, TotalScore, Level, ScoreList, SettingsTable, &LastClearImage, LineTypes, BigFont, &TetrominoImages, Blank, CurrentMode, Engine, UpcommingTetromino, HoldState);

					MoveDownTimer = Time;
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
				}
				if(KeyStates[Buttons::HoldSpot]) {
					if(!HoldState) {
						std::swap(HoldTetromino, MainTetromino);
						if(MainTetromino->GetRotation() == -1) {
							if(SettingsTable[2 /* 2 = Pentomino Setting */] == 0 /* 0=false */) {
							MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 3, CurrentMode);
							MainTetromino->SetImages(&TetrominoImages);
						}
						else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
							std::uniform_int_distribution<int> SizeSelect(3,5);
							std::mt19937 Engine;
							int SelectedLength = (floor((SizeSelect(Engine) - 3) / 2) + 3);
							MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, SelectedLength, CurrentMode);
							MainTetromino->SetImages(&TetrominoImages);
						}
						else if(SettingsTable[2] == 2 /* 2=true */) {
							MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 4, CurrentMode);
							MainTetromino->SetImages(&TetrominoImages);
						}
						}
						MainTetromino->SetLocation(5, 22);
						Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
						HoldTetromino->SetLocation(2, 2);
						UpcommingTetromino.SetImages(&TetrominoImages);
						UpcommingTetromino.SetLocation(2,2);
						HoldState = true;
					}
				}
			}
			else if(CurrentState == States::Menu) {
				if(KeyStates[Buttons::Return]) {
					MainWorld.Reset();
					MainWorld.SetImages(&TetrominoImages, &Blank);
					TotalLines = 0;
					TotalScore = 0;
					if(SettingsTable[2 /* 3 = Pentomino Setting */] == 0 /* 0=false */) {
						MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 3, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
						std::uniform_int_distribution<int> SizeSelect(3,5);
						std::mt19937 Engine;
						int SelectedLength = (floor((SizeSelect(Engine) - 3) / 2) + 3);
						MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, SelectedLength, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					else if(SettingsTable[2] == 2 /* 2=true */) {
						MainTetromino->ResetWithUpcomming(Engine,UpcommingTetromino, 4, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					UpcommingTetromino.SetImages(&TetrominoImages);
					UpcommingTetromino.SetLocation(2,2);
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
					CurrentState = States::Game;
				}
			}
			else if(CurrentState == States::Dead && KeyStates[Buttons::Return]) {
				HighscoresTable[5] = TotalScore;
				std::sort(HighscoresTable, HighscoresTable+6, [](long a, long b){return a>b;});

				switch(CurrentMode) {
					case Modes::Standard:
						File StandardHighscoresFile;
						StandardHighscoresFile.OpenFile("Data/StandardHighscores.bin", FileModes::Write | FileModes::Binary, HighscoresTable, 5, long);
						StandardHighscoresFile.Write(HighscoresTable, long, 5);
						StandardHighscoresFile.CloseFile();
						break;
				}

				CurrentState = States::Menu;
				LastClearImage.LoadFromText(" ", BigFont, Render, {255, 255, 255, 255});
				TotalLines = 0;
				TotalScore = 0;
				HoldTetromino->ResetShape(Engine, -1, CurrentMode);
			}
			
			if(MainWorld.LinesAbove(20) > 0 && CurrentState == States::Game) {
				CurrentState = States::Dead;
			}

			//Drawing Logic

			SDL_RenderClear(Render);

			Background.Draw(0, 0, Render);

			SurroundingLine.Draw(GAME_X-10, SCREEN_HEIGHT - 28*25 - 10, Render);

			LastClearImage.Draw(GAME_X + 28*11, 20, Render);

			std::string TotalLinesString, ScoreString;
			std::stringstream TotalLinesStringStream, ScoreStringStream;

			TotalLinesStringStream << TotalLines;
			TotalLinesString = TotalLinesStringStream.str();
			int w,h;
			TTF_SizeUTF8(SmallFont, TotalLinesString.c_str(), &w, &h);
			WriteNumber(Render, TotalLines, Numbers, GAME_X - w - 10, 20, SmallFont);

			ScoreStringStream << TotalScore;
			ScoreString = ScoreStringStream.str();
			TTF_SizeUTF8(SmallFont, ScoreString.c_str(), &w, &h);
			WriteNumber(Render, TotalScore, Numbers, GAME_X - (w) - 10, 41, SmallFont);



			//Heaight of text = 21

			TotalLinesText[0].Draw(Text_X, 20, Render);
			Score[0].Draw(Text_X, 41, Render);
			
			
			LevelText.Draw(Text_X, 20+(21*2), Render);
			std::string LevelString;
			std::stringstream LevelStringStream;

			LevelStringStream << Level;
			LevelString = LevelStringStream.str();
			TTF_SizeUTF8(SmallFont, LevelString.c_str(), &w, &h);
			WriteNumber(Render, Level, Numbers, GAME_X - (w) - 10, 20+(21*2), SmallFont);


			HighScoreTitle.Draw(Text_X+3, SCREEN_HEIGHT - (6*HighScoreTitle.GetSize().h), Render);
			for(int i=0; i<5; i++) {
				std::stringstream Temp1, Temp2;
				Temp2 << HighscoresTable[i];

				int w, h;
				TTF_SizeUTF8(SmallFont, Temp2.str().c_str(), &w, &h);

				WriteNumber(Render, HighscoresTable[i], Numbers, GAME_X - w - 10, SCREEN_HEIGHT - ((5-i)*h), SmallFont);
				HighscoresText[i].Draw(Text_X+3, SCREEN_HEIGHT - ((5-i)*h), Render);
			}


			if(CurrentState == States::Game) {


				MainWorld.Draw(Render, GAME_X, 0);
				MainTetromino->Draw(Render, GAME_X, 0);
				Ghost.Draw(Render, GAME_X, 0);

				HoldSpotBackground.Draw(Render, Text_X, 300);
				Outline.Draw(Text_X - 5, 300 - 14 - 5, Render);
				if(HoldTetromino->GetRotation() != -1) {
					HoldTetromino->Draw(Render, Text_X, 300);
				}

				UpcommingBackground.Draw(Render, Text_X, (720 - 460) - 5 * 28);
				//the way Worlds and Tetrominos are drawn is very different from the way images are
				//specificaly the 0,0 point is in the bottom left corner for Worlds and Tetrominos instead of the top left
				//AND the corner of the World/Tetromino that is actually at the specified cordinate also follows the above rule
				Outline.Draw(Text_X - 3, 460 - 3, Render);
				UpcommingTetromino.Draw(Render, Text_X, (720 - 460) - 5 * 28);
			}
			else if(CurrentState == States::Menu) {
				EmptyWorld.Draw(Render, GAME_X, 0);
			}
			else if(CurrentState == States::Dead) {
				MainWorld.Draw(Render, GAME_X, 0);
				DeathText.Draw(GAME_X + 140 - DeathText.GetSize().w/2, 100, Render);
			}

			DeadLine.Draw(GAME_X, 156, Render);

			SDL_RenderPresent(Render);
			SDL_Delay(0);
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
				if(TTF_Init() == -1) {
					std::cout << "Unable To Load Fontloader, TTF_ERROR: " << TTF_GetError() << '\n';
					return false;
				}
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

	TTF_Quit();

	IMG_Quit();

	SDL_Quit();

	std::cout << "exited succesfully\n";
}
