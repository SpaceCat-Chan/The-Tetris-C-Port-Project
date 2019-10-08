/*
TODO:
Add Basic UI
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
#include <SDL2/SDL_ttf.h>
#include <random>
#include <memory>
#include <math.h>
#include <time.h>
#include <sstream>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 720
#define yeet return 1

#define GAME_X 230
#define Text_X 20

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

#include "../SDL-Helper-Libraries/KeyHandlers/KeyHandlers.hpp"

#include "../SDL-Helper-Libraries/File/File.hpp"

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

		Image TotalLinesText[2], Score[2], LevelText[2];
		TotalLinesText[0].LoadFromText("TotalLines: ", SmallFont, Render, {255, 255, 255, 255});
		Score[0].LoadFromText("Score: ", SmallFont, Render, {255, 255, 255, 255});
		LevelText[0].LoadFromText("LevelText: ", SmallFont, Render, {255, 255, 255, 255});


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
		long StandardHighscoresTable[5];

		File StandardHighscoresFile;
		if(!StandardHighscoresFile.OpenFile("Data/StandardHighscores.bin", FileModes::Read | FileModes::Binary, StandardHighscoresTableStandard, 5, long)) {
			SDL_Log("FileError: %s\n", StandardHighscoresFile.GetError().c_str());
		}
		if(!StandardHighscoresFile.Read(StandardHighscoresTable, long, 5)) {
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

		if(ControlsFile.CloseFile()) {
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
			SDL_Log("FileError %s\n", SettingsFile.GetError().c_str());
		}

		KeyHandler KeyHandlerList[AmountOfControls + 2];

		KeyHandlerList[Buttons::MovLeft].SetValues(SettingsTable[1], SettingsTable[0], true);
		KeyHandlerList[Buttons::MovRight].SetValues(SettingsTable[1], SettingsTable[0], true);
		KeyHandlerList[Buttons::SoftDR].SetValues(SettingsTable[1], SettingsTable[0], true);

		unsigned long LastTime;
		LastTime = SDL_GetTicks();

		unsigned long Level=1, MoveDownTimer=0, TotalLines=0, TotalScore=0, LastMove=10;
		std::string LineTypes[8] = {"Single","Double","Triple","Tetris!","SUPER\nTETRIS!","T-Spin\nSingle","T-Spin\nDouble!","T-SPIN\nTRIPLE!"};
		std::string LastLineClear=" ";

		unsigned long ScoreList[] = {Single, Double, Triple, Tetris, STetris};

		while(!Quit) {
			while(SDL_PollEvent(&Event_Handler)) {
				if(Event_Handler.type == SDL_QUIT) {
					Quit = true;
				}
			}


			//button handeling
			const unsigned char* PressedKeys;
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

			//Logic handeling (timers, moving things, menues, all that stuff)

			Level = TotalLines/10;

			if(CurrentState == States::Game) {
				if(MoveDownTimer > ((85.52 * pow(0.88, Level)) * 10)) {
					if(MainTetromino->MoveDown(&MainWorld)) {
						std::unique_ptr<int[]> Check;
						int Amount;
						Amount = MainWorld.CheckLines(Check);
						for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
							MainWorld.ClearLine((Check)[i]);
						}
						bool IsTSpin=false;
						TotalLines += (Amount);
						if(Amount > 0) {
							TotalScore += (ScoreList[Amount-1] * (Level+1) * (IsTSpin ? Spin_Multiplier : 1));
						}
						//TODO: add t-spin detection

						if(Amount != 0) {
							LastLineClear = LineTypes[Amount - 1 + IsTSpin*5];
						}
						else {
							LastLineClear = " ";
						}
						if(SettingsTable[2 /* 2 = Pentomino Setting */] == 0 /* 0=false */) {
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
							MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 4, CurrentMode);
							MainTetromino->SetLocation(5, 22);
							MainTetromino->SetImages(&TetrominoImages);
						}
						MainWorld.SetImages(&TetrominoImages, &Blank);
						HoldState = false;
						Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
						UpcommingTetromino.SetImages(&TetrominoImages);
						UpcommingTetromino.SetLocation(2,2);
					}
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
						Down = MainTetromino->MoveDown(&MainWorld);
						if(Down) {
							std::unique_ptr<int[]> Check;
							int Amount;
							Amount = MainWorld.CheckLines(Check);
							for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
								MainWorld.ClearLine((Check)[i]);
							}
							bool IsTSpin=false;
							TotalLines += (Amount);
							if(Amount > 0) {
								TotalScore += ScoreList[Amount-1] * (Level+1) * (IsTSpin ? Spin_Multiplier : 1);
							}

							//TODO: add t-spin detection

							if(Amount != 0) {
								LastLineClear = LineTypes[Amount - 1 + IsTSpin*5];
							}
							else {
								LastLineClear = " ";
							}
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
								MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 4, CurrentMode);
								MainTetromino->SetLocation(5, 22);
								MainTetromino->SetImages(&TetrominoImages);
							}
							MainWorld.SetImages(&TetrominoImages, &Blank);
						}
					}
					while(!Down);
					HoldState = false;
					MoveDownTimer = 0;
					Ghost = MainTetromino->MakeGhost(MainWorld, &GhostImages);
					UpcommingTetromino.SetImages(&TetrominoImages);
					UpcommingTetromino.SetLocation(2,2);
				}
				if(KeyStates[Buttons::SoftDR]) {
					if(MainTetromino->MoveDown(&MainWorld)) {
						std::unique_ptr<int[]> Check;
						int Amount;
						Amount = MainWorld.CheckLines(Check);
						for(int i=(Amount-1); (i >= 0) && (Amount > 0); i--) {
							MainWorld.ClearLine((Check)[i]);
						}
						bool IsTSpin=false;
						TotalLines += (Amount);
						if(Amount > 0) {
							TotalScore += ScoreList[Amount-1] * (Level+1) * (IsTSpin ? Spin_Multiplier : 1);
						}

						//TODO: add t-spin detection

						if(Amount != 0) {
							LastLineClear = LineTypes[Amount - 1 + IsTSpin*5];
						}
						else {
							LastLineClear = " ";
						}
						if(SettingsTable[2 /* 2 = Pentomino Setting */] == 0 /* 0=false */) {
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
							MainTetromino->ResetWithUpcomming(Engine, UpcommingTetromino, 4, CurrentMode);
							MainTetromino->SetLocation(5, 22);
							MainTetromino->SetImages(&TetrominoImages);
						}
						MainWorld.SetImages(&TetrominoImages, &Blank);
						UpcommingTetromino.SetImages(&TetrominoImages);
						UpcommingTetromino.SetLocation(2,2);
						HoldState = false;
					}
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
			else if(CurrentState == States::Dead && PressedKeys[SDL_SCANCODE_RETURN]) {
				CurrentState = States::Menu;
				LastLineClear = " ";
				TotalLines = 0;
				TotalScore = 0;
			}
			
			if(MainWorld.LinesAbove(20) > 0 && CurrentState == States::Game) {
				CurrentState = States::Dead;
			}

			//Drawing Logic

			SDL_RenderClear(Render);

			Background.Draw(0, 0, Render);

			SurroundingLine.Draw(GAME_X-10, SCREEN_HEIGHT - 28*25 - 10, Render);

			Image LastClearImage;
			LastClearImage.LoadFromText(LastLineClear, BigFont, Render, {255, 255, 255, 255});
			LastClearImage.Draw(Text_X, 20, Render);

			std::string TotalLinesString, ScoreString;
			std::stringstream TotalLinesStringStream, ScoreStringStream;

			TotalLinesStringStream << TotalLines;
			TotalLinesString = TotalLinesStringStream.str();
			TotalLinesText[1].LoadFromText(TotalLinesString, SmallFont, Render, {255, 255, 255, 255});

			ScoreStringStream << TotalScore;
			ScoreString = ScoreStringStream.str();
			Score[1].LoadFromText(ScoreString, SmallFont, Render, {255, 255, 255, 255});


			//Heaight of text = 21

			TotalLinesText[0].Draw(Text_X, 20+(21*5), Render);
			TotalLinesText[1].Draw(GAME_X - (TotalLinesText[1].GetSize().w) - 10, 20+(21*5), Render);
			Score[0].Draw(Text_X, 20+(21*6), Render);
			Score[1].Draw(GAME_X - (Score[1].GetSize().w) - 10, 20+(21*6), Render);

			if(CurrentState == States::Game) {


				MainWorld.Draw(Render, GAME_X, 0);
				MainTetromino->Draw(Render, GAME_X, 0);
				Ghost.Draw(Render, GAME_X, 0);
				DeadLine.Draw(GAME_X, 156, Render);

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
