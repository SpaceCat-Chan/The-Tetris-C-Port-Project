/*
TODO:
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
#include <SDL.h>
#include <SDL_filesystem.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <random>
#include <memory>
#include <math.h>
#include <time.h>
#include <sstream>
#include <algorithm>

#include "../SDL-Helper-Libraries/Profiler.cpp"

constexpr unsigned int ScreenWidth=800;
constexpr unsigned int ScreenHeight=720;

constexpr unsigned int PlayAreaX=230;
constexpr unsigned int TextX=20;


constexpr int Single=1;
constexpr int Double=3;
constexpr int Triple=10;
constexpr int Tetris=32;
constexpr int STetris=70;
constexpr int SpinMultiplier=4;


constexpr int Puyo_Types=5;
constexpr int ChainLength=7;
constexpr int MaxTypesInOnePiece=2;

constexpr int TimeBeetweenAction=200;
//ProtSettings Finished

constexpr unsigned int AmountOfControls=8;
constexpr unsigned int AmountOfSettings=3;

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

struct BasicGameData {
	SDL_Renderer *Render=nullptr;
	SDL_Window *Window=nullptr;
	std::vector<TTF_Font*> Fonts;
	unsigned long CurrentScore=0, CurrentLevel=0, SelectedLevel=0, AmountLinesCleared=0;
	bool Quit=false;
	std::mt19937 RandEngine;
};

void DrawNumber(SDL_Renderer *Render, long Number, Image *Numbers, int x, int y, TTF_Font *font) {
	ProfileFunction();
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


bool ClearLines(BasicGameData &GameData, Tetromino *MainTetromino, World *MainWorld, unsigned long *ScoreList, long *SettingsTable, Image* LastClearImage, std::string *LineTypes, std::unique_ptr<Image[]> &TetrominoImages, Image &Blank, int &CurrentMode, Tetromino &UpcommingTetromino, bool &HoldState) {
	ProfileFunction();
	if(MainTetromino->MoveDown(MainWorld)) {
		std::unique_ptr<int[]> Check;
		int Amount;
		Amount = MainWorld->CheckLines(Check);
		for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
			MainWorld->ClearLine((Check)[i]);
		}

		//TODO: add t-spin detection

		bool IsTSpin=false;
		GameData.AmountLinesCleared += (Amount);
		if(Amount > 0) {
			GameData.CurrentScore += ScoreList[Amount-1] * (GameData.CurrentLevel+1) * (IsTSpin ? SpinMultiplier : 1);
			LastClearImage->LoadFromText(LineTypes[Amount - 1 + IsTSpin*5], GameData.Fonts[1], GameData.Render, {255, 255, 255, 255});
		}
		else {
			LastClearImage->LoadFromText(" ", GameData.Fonts[1], GameData.Render, {255, 255, 255, 255});
		}

		switch(SettingsTable[2 /*pentomino settings*/]) {
			case 0: { //never
				MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, 3, CurrentMode);
				MainTetromino->SetLocation(5, 22);
				MainTetromino->SetImages(&TetrominoImages);
				break;}
			
			case 1: { //sometimes
				std::uniform_int_distribution<int> SizeSelect(3,5);
				int SelectedLength = (floor((SizeSelect(GameData.RandEngine) - 3) / 2) + 3);
				MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, SelectedLength, CurrentMode);
				MainTetromino->SetLocation(5, 22);
				MainTetromino->SetImages(&TetrominoImages);
				break;}

			case 2: { //always
				MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, 4, CurrentMode);
				MainTetromino->SetLocation(5, 22);
				MainTetromino->SetImages(&TetrominoImages);
				break;}
		}

		MainWorld->SetImages(&TetrominoImages, &Blank);
		UpcommingTetromino.SetImages(&TetrominoImages);
		UpcommingTetromino.SetLocation(2,2);
		HoldState = false;
		return true;
	}
	return false;
}


bool init(BasicGameData &GameData);

void close(BasicGameData &GameData);

int main(int argc, char* argv[]) {
	StartProfiler("Tetris");
	ProfileFunction();
	BasicGameData GameData;

	if(init(GameData)) {

		SDL_Event Event_Handler;

		std::unique_ptr<Image[]> TetrominoImages;
		TetrominoImages = std::make_unique<Image[]>(TetrominoList::TOTAL_AMOUNT);

		SDL_Log("Loading Main Images\n");

		TetrominoImages[TetrominoList::S].LoadImage("Minos/SPiece.png", GameData.Render);
		TetrominoImages[TetrominoList::Z].LoadImage("Minos/ZPiece.png", GameData.Render);
		TetrominoImages[TetrominoList::L].LoadImage("Minos/LPiece.png", GameData.Render);
		TetrominoImages[TetrominoList::J].LoadImage("Minos/JPiece.png", GameData.Render);
		TetrominoImages[TetrominoList::I].LoadImage("Minos/IPiece.png", GameData.Render);
		TetrominoImages[TetrominoList::T].LoadImage("Minos/TPiece.png", GameData.Render);
		TetrominoImages[TetrominoList::O].LoadImage("Minos/OPiece.png", GameData.Render);

		std::unique_ptr<Image[]> GhostImages;
		GhostImages = std::make_unique<Image[]>(TetrominoList::TOTAL_AMOUNT);

		SDL_Log("Loading Ghost Images\n");

		GhostImages[TetrominoList::S].LoadImage("Minos/SPieceGhost.png", GameData.Render);
		GhostImages[TetrominoList::Z].LoadImage("Minos/ZPieceGhost.png", GameData.Render);
		GhostImages[TetrominoList::L].LoadImage("Minos/LPieceGhost.png", GameData.Render);
		GhostImages[TetrominoList::J].LoadImage("Minos/JPieceGhost.png", GameData.Render);
		GhostImages[TetrominoList::I].LoadImage("Minos/IPieceGhost.png", GameData.Render);
		GhostImages[TetrominoList::T].LoadImage("Minos/TPieceGhost.png", GameData.Render);
		GhostImages[TetrominoList::O].LoadImage("Minos/OPieceGhost.png", GameData.Render);

		SDL_Log("Loading other images\n");

		Image Blank;
		Blank.LoadImage("Minos/Blank.png", GameData.Render);

		Image Background, Title, SurroundingLine, DeadLine, Outline;
		Background.LoadImage("BackGround.png", GameData.Render);
		Title.LoadImage("Title.png", GameData.Render);
		SurroundingLine.LoadImage("Surrounding_Line.png", GameData.Render);
		DeadLine.LoadImage("Deadline.png", GameData.Render);
		Outline.LoadImage("5_5_Outline.png", GameData.Render);

		World UpcommingBackground{5,5}, HoldSpotBackground{5,5};
		UpcommingBackground.SetImages(&TetrominoImages, &Blank);
		HoldSpotBackground.SetImages(&TetrominoImages, &Blank);

		Background.SetColor(128, 128, 128);

		SDL_Log("finished loading images\n");

		GameData.Fonts.push_back(TTF_OpenFont("DejaVuSans.ttf", 18));
		GameData.Fonts.push_back(TTF_OpenFont("DejaVuSans.ttf", 36));

		if(!GameData.Fonts[0] || !GameData.Fonts[1]) {
			SDL_Log("unable to load Fonts, TTF_ERROR: %s\n", TTF_GetError());
		}

		Image DeathText, PauseText;
		DeathText.LoadFromText("You Died", GameData.Fonts[1], GameData.Render, {0xff, 0xff, 0xff, 0xff});
		PauseText.LoadFromText("Enter To Resume", GameData.Fonts[1], GameData.Render, {0xff, 0xff, 0xff, 0xff});

		Image TotalLinesText[2], Score[2], LevelText, LevelDecideArrows;
		TotalLinesText[0].LoadFromText("TotalLines: ", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Score[0].LoadFromText("Score: ", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		LevelText.LoadFromText("Level: ", GameData.Fonts[0], GameData.Render, {255, 255, 255});
		LevelDecideArrows.LoadImage("LevelDecideArrows.png", GameData.Render);


		SDL_Log("Finished loading Fonts and Static Text\n");

		World MainWorld, EmptyWorld;
		Tetromino MainSet, HoldSet, Ghost, UpcommingTetromino;
		Tetromino *MainTetromino, *HoldTetromino;

		MainTetromino = &MainSet;
		HoldTetromino = &HoldSet;

		MainWorld.SetImages(&TetrominoImages, &Blank);
		EmptyWorld.SetImages(&TetrominoImages, &Blank);

		GameData.RandEngine.seed(time(nullptr));

		MainTetromino->ResetShape(GameData.RandEngine);
		MainTetromino->SetLocation(5, 22);
		MainTetromino->SetImages(&TetrominoImages);
		UpcommingTetromino.SetImages(&TetrominoImages);
		HoldTetromino->SetImages(&TetrominoImages);

		bool HoldState;
		HoldState = false;

		int CurrentMode = Modes::Standard;
		int CurrentState = States::Menu;

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

		unsigned long MoveDownTimer=0;
		std::string LineTypes[8] = {"Single","Double","Triple","Tetris!","SUPER\nTETRIS!","T-Spin\nSingle","T-Spin\nDouble!","T-SPIN\nTRIPLE!"};
		LastClearImage.LoadFromText(" ", GameData.Fonts[1], GameData.Render, {255, 255, 255, 255});

		unsigned long ScoreList[] = {Single, Double, Triple, Tetris, STetris};

		Image HighScoreTitle, HighscoresText[5];
		HighscoresText[0].LoadFromText("1:", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		HighscoresText[1].LoadFromText("2:", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		HighscoresText[2].LoadFromText("3:", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		HighscoresText[3].LoadFromText("4:", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		HighscoresText[4].LoadFromText("5:", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		HighScoreTitle.LoadFromText("Highscores:", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		
		Image Numbers[10];
		Numbers[0].LoadFromText("0", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[1].LoadFromText("1", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[2].LoadFromText("2", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[3].LoadFromText("3", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[4].LoadFromText("4", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[5].LoadFromText("5", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[6].LoadFromText("6", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[7].LoadFromText("7", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[8].LoadFromText("8", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});
		Numbers[9].LoadFromText("9", GameData.Fonts[0], GameData.Render, {255, 255, 255, 255});

		bool PressedKeys[AmountOfControls + 2];
		int PressedFor[AmountOfControls + 2];
		for(unsigned int i=0; i<AmountOfControls+2; i++) {
			PressedKeys[i] = false;
			PressedFor[i] = 0;
		}

		while(!GameData.Quit) {
			Profile("GameLoop");
			unsigned long Time, Temp;
			Temp = SDL_GetTicks();
			Time = Temp - LastTime;
			LastTime = Temp;

			Uint8 KeyStates[AmountOfControls + 2], AlreadyLetGo[AmountOfControls + 2];
			for(unsigned int i=0; i<AmountOfControls+2; i++) {
				KeyStates[i] = false;
				AlreadyLetGo[i] = false;
			}

			while(SDL_PollEvent(&Event_Handler)) {
				if(Event_Handler.type == SDL_QUIT) {
					GameData.Quit = true;
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
				if(Event_Handler.type == SDL_MOUSEBUTTONUP) {
					if(CurrentState == States::Menu) {
						if(Event_Handler.button.x > (int)TextX && Event_Handler.button.x < (int)TextX+25) {
							if(Event_Handler.button.y > 20+(21*4) && Event_Handler.button.y < 20+(21*4) + 33) {
								GameData.SelectedLevel++;
							}
							else if(Event_Handler.button.y > 20+(21*4) + 57 && Event_Handler.button.y < 20+(21*4) + 90)
							{
								if(GameData.SelectedLevel != 0)
									GameData.SelectedLevel--;
							}
						}
					}
				}
			}

			for(unsigned int i=0; i<AmountOfControls+2; i++) {
				if(PressedKeys[i]) {
					PressedFor[i] += Time;
				}
			}

			for(unsigned int i=0; i < AmountOfControls + 2; i++) {
				if(PressedFor[i] > SettingsTable[0]+SettingsTable[1]) {
					if(i == Buttons::SoftDR || i == Buttons::MovLeft || i == Buttons::MovRight) {
						KeyStates[i] = true;
						PressedFor[i] = SettingsTable[0];
					}
				}
			}
			
			//Logic handeling (timers, moving things, menues, all that stuff)

			GameData.CurrentLevel = std::max(GameData.AmountLinesCleared/10, GameData.SelectedLevel);

			if(KeyStates[Buttons::Escape] && CurrentState == States::Game) {
				CurrentState = States::Pause;
			}

			if(KeyStates[Buttons::Return] && CurrentState == States::Pause) {
				CurrentState = States::Game;
			}

			if(CurrentState == States::Game) {
				if(MoveDownTimer > ((85.52 * pow(0.88, GameData.CurrentLevel)) * 10)) {
					if(ClearLines(GameData, MainTetromino, &MainWorld, ScoreList, SettingsTable, &LastClearImage, LineTypes, TetrominoImages, Blank, CurrentMode, UpcommingTetromino, HoldState))
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
						Down = ClearLines(GameData, MainTetromino, &MainWorld, ScoreList, SettingsTable, &LastClearImage, LineTypes, TetrominoImages, Blank, CurrentMode, UpcommingTetromino, HoldState);
					}
					while(!Down);
					HoldState = false;
					MoveDownTimer = 0;
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
					UpcommingTetromino.SetImages(&TetrominoImages);
					UpcommingTetromino.SetLocation(2,2);
				}
				if(KeyStates[Buttons::SoftDR]) {
					ClearLines(GameData, MainTetromino, &MainWorld, ScoreList, SettingsTable, &LastClearImage, LineTypes, TetrominoImages, Blank, CurrentMode, UpcommingTetromino, HoldState);

					MoveDownTimer = Time;
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
				}
				if(KeyStates[Buttons::HoldSpot]) {
					if(!HoldState) {
						std::swap(HoldTetromino, MainTetromino);
						if(MainTetromino->GetRotation() == -1) {
							if(SettingsTable[2 /* 2 = Pentomino Setting */] == 0 /* 0=false */) {
							MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, 3, CurrentMode);
							MainTetromino->SetImages(&TetrominoImages);
						}
						else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
							std::uniform_int_distribution<int> SizeSelect(3,5);
							int SelectedLength = (floor((SizeSelect(GameData.RandEngine) - 3) / 2) + 3);
							MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, SelectedLength, CurrentMode);
							MainTetromino->SetImages(&TetrominoImages);
						}
						else if(SettingsTable[2] == 2 /* 2=true */) {
							MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, 4, CurrentMode);
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
					GameData.AmountLinesCleared = 0;
					GameData.CurrentScore = 0;
					if(SettingsTable[2 /* 3 = Pentomino Setting */] == 0 /* 0=false */) {
						MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, 3, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					else if(SettingsTable[2] == 1 /* 1=Sometimes */) {
						std::uniform_int_distribution<int> SizeSelect(3,5);
						int SelectedLength = (floor((SizeSelect(GameData.RandEngine) - 3) / 2) + 3);
						MainTetromino->ResetWithUpcomming(GameData.RandEngine, UpcommingTetromino, SelectedLength, CurrentMode);
						MainTetromino->SetLocation(5, 22);
						MainTetromino->SetImages(&TetrominoImages);
					}
					else if(SettingsTable[2] == 2 /* 2=true */) {
						MainTetromino->ResetWithUpcomming(GameData.RandEngine,UpcommingTetromino, 4, CurrentMode);
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
				HighscoresTable[5] = GameData.CurrentScore;
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
				LastClearImage.LoadFromText(" ", GameData.Fonts[1], GameData.Render, {255, 255, 255, 255});
				GameData.AmountLinesCleared = 0;
				GameData.CurrentScore = 0;
				HoldTetromino->ResetShape(GameData.RandEngine, -1, CurrentMode);
			}
			
			if(MainWorld.LinesAbove(20) > 0 && CurrentState == States::Game) {
				CurrentState = States::Dead;
			}

			//Drawing Logic

			SDL_RenderClear(GameData.Render);

			Background.Draw(0, 0, GameData.Render);

			SurroundingLine.Draw(PlayAreaX-10, ScreenHeight - 28*25 - 10, GameData.Render);

			LastClearImage.Draw(PlayAreaX + 28*11, 20, GameData.Render);

			std::string TotalLinesString, ScoreString;
			std::stringstream TotalLinesStringStream, ScoreStringStream;

			TotalLinesStringStream << GameData.AmountLinesCleared;
			TotalLinesString = TotalLinesStringStream.str();
			int w,h;
			TTF_SizeUTF8(GameData.Fonts[0], TotalLinesString.c_str(), &w, &h);
			DrawNumber(GameData.Render, GameData.AmountLinesCleared, Numbers, PlayAreaX - w - 10, 20, GameData.Fonts[0]);

			ScoreStringStream << GameData.CurrentScore;
			ScoreString = ScoreStringStream.str();
			TTF_SizeUTF8(GameData.Fonts[0], ScoreString.c_str(), &w, &h);
			DrawNumber(GameData.Render, GameData.CurrentScore, Numbers, PlayAreaX - (w) - 10, 41, GameData.Fonts[0]);



			//Heaight of text = 21

			TotalLinesText[0].Draw(TextX, 20, GameData.Render);
			Score[0].Draw(TextX, 41, GameData.Render);
			
			
			LevelText.Draw(TextX, 20+(21*2), GameData.Render);
			std::string LevelString;
			std::stringstream LevelStringStream;

			LevelStringStream << GameData.CurrentLevel;
			LevelString = LevelStringStream.str();
			TTF_SizeUTF8(GameData.Fonts[0], LevelString.c_str(), &w, &h);
			DrawNumber(GameData.Render, GameData.CurrentLevel, Numbers, PlayAreaX - (w) - 10, 20+(21*2), GameData.Fonts[0]);


			HighScoreTitle.Draw(TextX+3, ScreenHeight - (6*HighScoreTitle.GetSize().h), GameData.Render);
			for(int i=0; i<5; i++) {
				std::stringstream Temp1, Temp2;
				Temp2 << HighscoresTable[i];

				int w, h;
				TTF_SizeUTF8(GameData.Fonts[0], Temp2.str().c_str(), &w, &h);

				DrawNumber(GameData.Render, HighscoresTable[i], Numbers, PlayAreaX - w - 10, ScreenHeight - ((5-i)*h), GameData.Fonts[0]);
				HighscoresText[i].Draw(TextX+3, ScreenHeight - ((5-i)*h), GameData.Render);
			}

			if(CurrentState == States::Game) {


				MainWorld.Draw(GameData.Render, PlayAreaX, 0);
				MainTetromino->Draw(GameData.Render, PlayAreaX, 0);
				Ghost.Draw(GameData.Render, PlayAreaX, 0);

				constexpr int HoldSpotY=150;
				constexpr int UpcommingY=300;

				HoldSpotBackground.Draw(GameData.Render, TextX, ScreenHeight - HoldSpotY - 5 * 28);
				if(HoldTetromino->GetRotation() != -1) {
					HoldTetromino->Draw(GameData.Render, TextX, ScreenHeight - HoldSpotY - 5 * 28);
				}
				Outline.Draw(TextX - 3, HoldSpotY - 5, GameData.Render);

				UpcommingBackground.Draw(GameData.Render, TextX, ScreenHeight - UpcommingY - 5 * 28);
				//the way Worlds and Tetrominos are drawn is very different from the way images are
				//specificaly the 0,0 point is in the bottom left corner for Worlds and Tetrominos instead of the top left
				//AND the corner of the World/Tetromino that is actually at the specified cordinate also follows the above rule
				
				//future me here, all i have to say is fuck you past me
				UpcommingTetromino.Draw(GameData.Render, TextX, (ScreenHeight - UpcommingY) - 5 * 28);
				Outline.Draw(TextX - 3, UpcommingY - 5, GameData.Render);
			}
			else if(CurrentState == States::Menu || CurrentState == States::Pause) {
				EmptyWorld.Draw(GameData.Render, PlayAreaX, 0);
			}
			if(CurrentState == States::Pause) {
				PauseText.Draw(PlayAreaX + 140 - (PauseText.GetSize().w*0.90)/2, 100, GameData.Render, {0, 0, 90, 90});
			}
			else if(CurrentState == States::Menu) {
				LevelDecideArrows.Draw(TextX, 20+(21*4), GameData.Render);
				Image LevelDecideString;
				std::stringstream ToString;
				ToString << GameData.SelectedLevel;
				LevelDecideString.LoadFromText(ToString.str(), GameData.Fonts[0], GameData.Render, {0xff, 0xff, 0xff, 0xff});
				LevelDecideString.Draw(TextX+LevelDecideArrows.GetSize().w/2-LevelDecideString.GetSize().w/2, 20+(21*4)+ 34, GameData.Render);
			}
			else if(CurrentState == States::Dead) {
				MainWorld.Draw(GameData.Render, PlayAreaX, 0);
				DeathText.Draw(PlayAreaX + 140 - DeathText.GetSize().w/2, 100, GameData.Render);
			}

			DeadLine.Draw(PlayAreaX, 156, GameData.Render);

			SDL_RenderPresent(GameData.Render);
			SDL_Delay(0);
		}
		close(GameData);
	}
	EndProfiler();
}

bool init(BasicGameData &GameData) {
	ProfileFunction();
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	
	GameData.Window = SDL_CreateWindow("Fook u", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
	if(GameData.Window == nullptr) {
		SDL_Log("Window could not be created, SDL_Error: %s\n", SDL_GetError());
		return false;
	}

		int Img_Flags = IMG_INIT_PNG;
	if(!(IMG_Init(Img_Flags) & Img_Flags)) {
		SDL_Log("Unable to load PNG loader, IMG_ERROR: %s\n", IMG_GetError());
		return false;
	}

	if(TTF_Init() == -1) {
		SDL_Log("Unable To Load Fontloader, TTF_ERROR: %s\n", TTF_GetError());
		return false;
	}

	GameData.Render = SDL_CreateRenderer(GameData.Window, -1, SDL_RENDERER_ACCELERATED);
	if(GameData.Render == nullptr) {
		std::cout << "unable to create Renderer, SDL_ERROR" << SDL_GetError() << '\n';
		return false;
	}
	return true;
}

void close(BasicGameData &GameData) {
	ProfileFunction();
	SDL_DestroyWindow(GameData.Window);
	SDL_DestroyRenderer(GameData.Render);

	for(auto& Font : GameData.Fonts) {
		TTF_CloseFont(Font);
	}

	TTF_Quit();

	IMG_Quit();

	SDL_Quit();

	SDL_Log("exited succesfully\n");
}