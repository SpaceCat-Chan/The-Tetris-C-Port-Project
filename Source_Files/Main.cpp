/*
TODO:
add line clear and line check functions to World (DONE)
draw graphics (DONE)
keep track of scores
add file handeling (IN PROGRESS)
set up settings and controls
add the other modes
add finishing touches

IMPORTANT:
Replace pointers with unique_ptr and make_unique
*/

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <random>
#include <memory>

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
	Swap=7;
}

class Image {
	SDL_Texture* ImageFile;
	int Width, Height;
	public:
	void Free() {
		if(ImageFile == nullptr) {
			SDL_free(ImageFile);
			ImageFile = nullptr;
			Width = 0;
			Height = 0;
		}
	}
	Image() {
		Height = 0;
		Width = 0;
		ImageFile = nullptr;
	}
	~Image() {
		Free();
	}
	bool LoadImage(std::string Path, SDL_Renderer* Render) {
		Free();
		SDL_Surface* TempImage = NULL;
		SDL_Texture* OptimizedTemp = NULL;
		TempImage = IMG_Load(Path.c_str());
		if(TempImage == NULL) {
			std::cout << "Image could not be loaded, SDL_ERROR: " << SDL_GetError() << "\nPATH: " << Path << '\n';
			return false;
		}
		else {
			OptimizedTemp = SDL_CreateTextureFromSurface(Render, TempImage);
			if(OptimizedTemp == NULL) {
				std::cout << "unable to Create Texture, SDL_ERROR: " << SDL_GetError() << "\nPATH: " << Path << '\n';
				SDL_FreeSurface(TempImage);
				return false;
			}
			else {
				Width = TempImage->w;
				Height = TempImage->h;
				ImageFile = OptimizedTemp;
				SDL_FreeSurface(TempImage);
				return true;
			}
		}
	}
	void Draw(int x, int y, SDL_Renderer* Render, SDL_Rect* clip = nullptr, double Angle = 0.0, SDL_Point* Center = nullptr, SDL_RendererFlip Flip = SDL_FLIP_NONE) {
		//Set rendering space and render to screen
		SDL_Rect renderQuad = { x, y, Width, Height };

		//Set clip rendering dimensions
		if( clip != NULL )
		{
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		//Render to screen
		SDL_RenderCopyEx( Render, ImageFile, clip, &renderQuad, Angle, Center, Flip );
	}

	void SetColor(long r, long g, long b) {
		SDL_SetTextureColorMod(ImageFile, r, g, b);
	}

	void SetAlpha(long a) {
		SDL_SetTextureAlphaMod(ImageFile, a);
	}
	
	void SetBlendMode(SDL_BlendMode BlendMode) {
		SDL_SetTextureBlendMode(ImageFile, BlendMode);
	}
};

class Mino {
	friend class Tetromino;
	friend class World;
	int Type=0;
	Image* ImageFile=nullptr;
	SDL_Point Position;
	bool Active;
	public:

	Mino() {
		Position.x = -1;
		Position.y = -1;
		Active = true;
	}

	Mino(int x, int y) {
		Position.x = x;
		Position.y = y;
		Active = true;
	}

	~Mino() = default;

	Mino(Mino& Copy) = default;

	Mino& operator=(Mino& Copy) = default;

	Mino(Mino&& Move) = default;

	Mino& operator=(Mino&& Move) = default;

	void SetImage(Image* LoadImage) {
		ImageFile = LoadImage;
	}
	void SetImageFromType(std::unique_ptr<Image[]>* ImageList, Image* InactiveImage = nullptr) {
		if(Active) {
			ImageFile = &((*ImageList)[Type]);
		}
		else {
			ImageFile = InactiveImage;
		}
	}
	SDL_Point GetPosition() {
		return Position;
	}
	void SetPosition(int x, int y) {
		Position.x = x;
		Position.y = y;
	}
	void SetType(int NewType) {
		Type = NewType;
	}
	void Draw(SDL_Renderer* Render, int X_Offset, int Y_Offset) {
		ImageFile->Draw(X_Offset + 28 * Position.x, SCREEN_HEIGHT - (Y_Offset + 28 * Position.y), Render);
	}
	bool GetActive() {
		return Active;
	}
};

SDL_Point RotatePoint(bool Dir, SDL_Point ToRotate) { //Dir: True = Counter Clockwise
	SDL_Point Rotated;
	if(Dir) {
		Rotated.x = -1 * ToRotate.y;
		Rotated.y = ToRotate.x;
	}
	else {
		Rotated.x = ToRotate.y;
		Rotated.y = -1 * ToRotate.x;
	}
	return(Rotated);
}

class World;

class Tetromino {
	friend class World;
	Mino MainPiece;
	std::unique_ptr<Mino[]> Pieces;
	int Type, Length, Rotation;
	public:
	
	Tetromino() {}

	~Tetromino() {}

	Tetromino(Tetromino& Copy) {
		if(Copy.Pieces != nullptr) {
			MainPiece = Copy.MainPiece;
			Type = Copy.Type;
			Length = Copy.Length;
			Pieces = std::make_unique<Mino[]>(Length);

			for(int i=0; i<Length; i++) {
				Pieces[i] = Copy.Pieces[i];
			}
		}

	}

	Tetromino& operator= (Tetromino& Copy) {
		MainPiece = Copy.MainPiece;
		Type = Copy.Type;
		Length = Copy.Length;
		Pieces = std::make_unique<Mino[]>(Length);

		for(int i=0; i<Length; i++) {
			Pieces[i] = Copy.Pieces[i];
		}
		return *this;
	}

	Tetromino(Tetromino&& Copy) {
		if(Copy.Pieces != nullptr) {
			MainPiece = Copy.MainPiece;
			Type = Copy.Type;
			Length = Copy.Length;
			Pieces = std::make_unique<Mino[]>(Length);

			for(int i=0; i<Length; i++) {
				Pieces[i] = Copy.Pieces[i];
			}
		}
	}

	Tetromino& operator= (Tetromino&& Copy) {
		MainPiece = Copy.MainPiece;
		Type = Copy.Type;
		Length = Copy.Length;
		Pieces = std::make_unique<Mino[]>(Length);

		for(int i=0; i<Length; i++) {
			Pieces[i] = Copy.Pieces[i];
		}
		return *this;
	}

	void ResetShape(int Length=3, int Mode=Modes::Standard);

	void SetLocation(int x, int y) {
		MainPiece.SetPosition(x, y);
	}
	Tetromino Rotate(bool Dir);

	void RotateSelf(bool Dir, World* TestWith);

	bool MoveDown(World* Testwith);

	void SetImages(std::unique_ptr<Image[]>* ImageList) {
		for(int i=0; i<Length; i++) {
			Pieces[i].SetImageFromType(ImageList);
		}
		MainPiece.SetImageFromType(ImageList);
	}

	void Draw(SDL_Renderer* Render, int X, int Y) {
		for(int i=0; i<Length; i++) {
			Pieces[i].Draw(Render, MainPiece.Position.x * 28 + X, MainPiece.Position.y * 28 + Y + 28);
		}
		MainPiece.Draw(Render, X, Y+28);
	}
	int GetRotation() {
		return Rotation;
	}
	void MoveSide(int Side, World* TestWith);
};

class World {
	Mino Random{-1, -1};
	std::unique_ptr<std::unique_ptr<Mino[]>[]> Info;
	int x,y;
	public:
	World() {
		Info = std::make_unique<std::unique_ptr<Mino[]>[]>(25);
		for(int i=0; i<25; i++) {
			Info[i] = std::make_unique<Mino[]>(10);
			for(int k=0; k<10; k++) {
				Info[i][k].Active = false;
				Info[i][k].Position.x = k;
				Info[i][k].Position.y = i;
			}
		}
		x = 10;
		y = 25;
		Random.Active = true;
	}
	World(int X, int Y) {
		Info = std::make_unique<std::unique_ptr<Mino[]>[]>(Y);
		for(int i=0; i<Y; i++) {
			Info[i] = std::make_unique<Mino[]>(X);
			for(int k=0; k<X; k++) {
				Info[i][k].Active = false;
				Info[i][k].Position.x = k;
				Info[i][k].Position.y = i;
			}
		}
		x = X;
		y = Y;
		Random.Active = true;
	}
	~World() {}
	Mino& GetSpot(int X, int Y) {
		if(X < 0 || X >= x || Y < 0 || Y>= y) {
			return Random;
		}
		return Info[Y][X];
	}
	void AbsorbTetromino(Tetromino& Absorb) {
		for(int i=0; i<Absorb.Length; i++) {
			Absorb.Pieces[i].Position.x += Absorb.MainPiece.Position.x;
			Absorb.Pieces[i].Position.y += Absorb.MainPiece.Position.y;
			Info[Absorb.Pieces[i].Position.y][Absorb.Pieces[i].Position.x] = Absorb.Pieces[i];
			Info[Absorb.Pieces[i].Position.y][Absorb.Pieces[i].Position.x].Active = true;

		}
		Info[Absorb.MainPiece.Position.y][Absorb.MainPiece.Position.x] = Absorb.MainPiece;
		Info[Absorb.MainPiece.Position.y][Absorb.MainPiece.Position.x].Active = true;

		Absorb.Pieces.reset();
		Mino Random;
		Absorb.MainPiece = Random;
	}
	void SetImages(std::unique_ptr<Image[]>* ImageList, Image* InactiveImage) {
		for(int i=0; i<y; i++) {
			for(int k=0; k<x; k++) {
				Info[i][k].SetImageFromType(ImageList, InactiveImage);
			}
		}
	}
	void Draw(SDL_Renderer* Render, int X, int Y) {
		for(int i=0; i<y; i++) {
			for(int k=0; k<x; k++) {
				Info[i][k].Draw(Render, X, Y+28);
			}
		}
	}
	//return = size of ReturnList
	int CheckLines(std::unique_ptr<int[]>* ReturnList);

	void ClearLine(int Line);
};

bool init(SDL_Window** window, SDL_Renderer** Render);

void close(SDL_Window* window, SDL_Renderer* Render);

class RepeatingKeyPress;

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

		Image Background, Title;
		Background.LoadImage("BackGround.png", Render);
		Title.LoadImage("Title.png", Render);

		Background.SetColor(128, 128, 128);

		std::cout << "finished loading images\n";

		World Test;
		Tetromino TestPiece;

		Test.SetImages(&TetrominoImages, &Blank);
		TestPiece.ResetShape();
		TestPiece.SetLocation(5, 22);
		TestPiece.SetImages(&TetrominoImages);

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
					if(SDL_RWwrite(StandardHighscores, &Temp, Size, 1) != Size) {
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
					if(SDL_RWwrite(ControlsFile, &RotLeft, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &RotRight, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &HardDR, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &SoftDR, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &MovLeft, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &MovRight, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &HoldSpot, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(ControlsFile, &SwapButton, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					
					SDL_RWclose(ControlsFile);
					SDL_RWFromFile("Data/Controls.bin", "rb");
				}
			}
		}

		long SettingsTable[AmountOfSettings];
		bool Pentomino;
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
					int Pentomino=0;
					
					size_t Size;
					Size = sizeof(int);
					if(SDL_RWwrite(SettingsFile, &AutoRepeat_Delay, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(SettingsFile, &AutoRepeat_Speed, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					if(SDL_RWwrite(SettingsFile, &Pentomino, Size, 1) != Size) {
						std::cout << "unable to write all data, SDL_ERROR: " << SDL_GetError() << '\n';
					}
					
					SDL_RWclose(SettingsFile);
					SDL_RWFromFile("Data/Settings.bin", "rb");
				}
			}
		}

		if(!Quit) {
			SDL_RWread(SettingsFile, SettingsTable, sizeof(uint8_t), 8);
			SDL_RWclose(SettingsFile);
		}

		RepeatingKeyPress MoveLeftKey, MoveRightKey, MoveDownKey;
		MoveLeftKey.SetValues(SettingsTable[2], SettingsTable[1]);
		MoveRightKey.SetValues(SettingsTable[2], SettingsTable[1]);
		MoveDownKey.SetValues(SettingsTable[2], SettingsTable[1]);

		unsigned long LastTime;
		LastTime = SDL_GetTicks();

		while(!Quit) {
			while(SDL_PollEvent(&Event_Handler)) {
				if(Event_Handler.type == SDL_QUIT) {
					Quit = true;
				}
				else if(Event_Handler.type == SDL_KEYDOWN) {
					if(Event_Handler.key.keysym.sym == ControlsTable[Buttons::MovLeft]) {
						MoveLeftKey.Press();
					}
					else if(Event_Handler.key.keysym.sym == ControlsTable[Buttons::MovRight]) {
						MoveRightKey.Press();
					}
					else if(Event_Handler.key.keysym.sym == ControlsTable[Buttons::SoftDR]) {
						MoveDownKey.Press();
					}
				}
				else if(Event_Handler.type == SDL_KEYUP) {
					if(Event_Handler.key.keysym.sym == ControlsTable[Buttons::MovLeft]) {
						MoveLeftKey.UnPress();
					}
					else if(Event_Handler.key.keysym.sym == ControlsTable[Buttons::MovRight]) {
						MoveRightKey.UnPress();
					}
					else if(Event_Handler.key.keysym.sym == ControlsTable[Buttons::SoftDR]) {
						MoveDownKey.UnPress();
					}
				}
			}
			const Uint8* PressedKeys;
			PressedKeys = SDL_GetKeyboardState(NULL);

			unsigned long Time, Temp;
			Temp = SDL_GetTicks();
			Time = Temp - LastTime;
			LastTime = Temp;

			Uint8 MoveLeft, MoveRight, MoveDown;

			MoveLeft = MoveLeftKey.Tick(Time);
			MoveRight = MoveRightKey.Tick(Time);
			MoveDown = MoveDownKey.Tick(Time);

			SDL_RenderClear(Render);

			Background.Draw(0, 0, Render);

			Test.Draw(Render, GAME_X, 0);
			TestPiece.Draw(Render, GAME_X, 0);
			SDL_RenderPresent(Render);

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

 void Tetromino::ResetShape(int length/*=3*/, int Mode/*=Modes::Standard*/) {
	//lot of templates here, just ignore it. i wish there was a way to make all this shorter/take up less space
	SDL_Point S_template[4];
	S_template[0].x = -1;
	S_template[0].y = 0;

	S_template[1].x = 0;
	S_template[1].y = 1;
		
	S_template[2].x = 1;
	S_template[2].y = 1;

	S_template[3].x = -2;
	S_template[3].y = 0;



	SDL_Point Z_template[4];
	Z_template[0].x = 1;
	Z_template[0].y = 0;

	Z_template[1].x = 0;
	Z_template[1].y = 1;
		
	Z_template[2].x = -1;
	Z_template[2].y = 1;

	Z_template[3].x = 1;
	Z_template[3].y = 1;


	SDL_Point L_template[4];
	L_template[0].x = -1;
	L_template[0].y = 0;

	L_template[1].x = 1;
	L_template[1].y = 0;
		
	L_template[2].x = 1;
	L_template[2].y = 1;

	L_template[3].x = -1;
	L_template[3].y = 1;


	SDL_Point J_template[4];
	J_template[0].x = 1;
	J_template[0].y = 0;

	J_template[1].x = -1;
	J_template[1].y = 0;
		
	J_template[2].x = -1;
	J_template[2].y = 1;

	J_template[3].x = 0;
	J_template[3].y = 1;


	SDL_Point I_template[4];
	I_template[0].x = 1;
	I_template[0].y = 0;

	I_template[1].x = 2;
	I_template[1].y = 0;
		
	I_template[2].x = -1;
	I_template[2].y = 0;

	I_template[3].x = -2;
	I_template[3].y = 0;


	SDL_Point T_template[4];
	T_template[0].x = -1;
	T_template[0].y = 0;

	T_template[1].x = 1;
	T_template[1].y = 0;
		
	T_template[2].x = 0;
	T_template[2].y = 1;

	T_template[3].x = 0;
	T_template[3].y = 2;


	SDL_Point O_template[4];
	O_template[0].x = 1;
	O_template[0].y = 0;

	O_template[1].x = 0;
	O_template[1].y = 1;
	
	O_template[2].x = 1;
	O_template[2].y = 1;

	O_template[3].x = 0;
	O_template[3].y = -1;

	std::uniform_int_distribution<int> dist(0,6);
	std::mt19937 Engine;
	Engine.seed(time(nullptr) * 1000);

	SDL_Point* Selected_Template;
	int RandomNum;
	RandomNum = dist(Engine);


	if(RandomNum == TetrominoList::S) {
		Selected_Template = S_template;
	}
	else if(RandomNum == TetrominoList::Z) {
		Selected_Template = Z_template;
	}
	else if(RandomNum == TetrominoList::L) {
		Selected_Template = L_template;
	}
	else if(RandomNum == TetrominoList::J) {
		Selected_Template = J_template;
	}
	else if(RandomNum == TetrominoList::I) {
		Selected_Template = I_template;
	}
	else if(RandomNum == TetrominoList::T) {
		Selected_Template = T_template;
	}
	else {
		Selected_Template = O_template;
	}

	Pieces = std::make_unique<Mino[]>(length);

	for(int i=0; i<length; i++) {
		Pieces[i].SetPosition(Selected_Template[i].x, Selected_Template[i].y);
	}

	MainPiece.SetPosition(0, 0);

	Type = RandomNum;

	if(Mode == Modes::Puyo) {
		std::uniform_int_distribution<int> PuyoDist(0, Puyo_Types - 1);

		std::unique_ptr<int[]> AllowedTypes;

		AllowedTypes = std::make_unique<int[]>(Max_Types_In_One_Piece);

		for(int i=0; i < Max_Types_In_One_Piece; i++) {
			AllowedTypes[i] = PuyoDist(Engine);
		}

		std::uniform_int_distribution<int> SelectFromAllowed{0, Max_Types_In_One_Piece - 1};

		for(int i=0; i<length; i++) {
			Pieces[i].SetType(SelectFromAllowed(Engine));
		}
		MainPiece.SetType(SelectFromAllowed(Engine));
	}
	else {
		for(int i=0; i<length; i++) {
			Pieces[i].SetType(Type);
		}
		MainPiece.SetType(Type);
	}
	Length = length;
	Rotation = Rotations::ORIGINAL;
}

Tetromino Tetromino::Rotate(bool Dir) {
	Tetromino Rotated;
	Rotated = (*this);

	for(int i=0; i<Length; i++) {
		Rotated.Pieces[i].Position = RotatePoint(Dir, (Rotated.Pieces[i].Position));
	}
	return Rotated;
}

std::unique_ptr<SDL_Point[]> SubtractPointLists(std::unique_ptr<SDL_Point[]>& A, std::unique_ptr<SDL_Point[]>& B) {
	std::unique_ptr<SDL_Point[]> Result;
	Result = std::make_unique<SDL_Point[]>(5);

	Result[0].x = A[0].x - B[0].x;
	Result[0].y = A[0].y - B[0].y;

	Result[1].x = A[1].x - B[1].x;
	Result[1].y = A[1].y - B[1].y;

	Result[2].x = A[2].x - B[2].x;
	Result[2].y = A[2].y - B[2].y;

	Result[3].x = A[3].x - B[3].x;
	Result[3].y = A[3].y - B[3].y;

	Result[4].x = A[4].x - B[4].x;
	Result[4].y = A[4].y - B[4].y;

	return Result;
}

void Tetromino::RotateSelf(bool Dir, World* TestWith) {
#include "SRS_File.cpp"

	std::unique_ptr<std::unique_ptr<SDL_Point[]>[]> Final_Offset;

	if(Type == TetrominoList::I) {
		Final_Offset = std::move(I_OffSet);
	}
	else if(Type == TetrominoList::O) {
		Final_Offset = std::move(O_OffSet);
	}
	else {
		Final_Offset = std::move(Other_OffSet);
	}

	int NewRotation;
	
	NewRotation = (!Dir) ? (Rotation + 1) : (Rotation - 1);

	if(NewRotation < Rotations::ORIGINAL) {
		NewRotation = Rotations::LEFT;
	}
	else if(NewRotation > Rotations::LEFT) {
		NewRotation = Rotations::ORIGINAL;
	}

	std::unique_ptr<SDL_Point[]> Translations;
	Translations = SubtractPointLists(Final_Offset[Rotation], Final_Offset[NewRotation]);

	bool Succes_Final = false;
	int Succesful_Translation = 0;

	for(int i=0; i<5; i++) {
		Tetromino TestTetromino;
		bool Current_Succes = true;

		TestTetromino = Rotate(Dir);
		TestTetromino.MainPiece.SetPosition(TestTetromino.MainPiece.GetPosition().x + Translations[i].x, TestTetromino.MainPiece.GetPosition().y + Translations[i].y);
		for(int k=0; k<TestTetromino.Length; k++) {
			if(TestWith->GetSpot(TestTetromino.Pieces[k].Position.x + TestTetromino.MainPiece.Position.x, TestTetromino.Pieces[k].Position.y + TestTetromino.MainPiece.Position.y).Active) {
				Current_Succes = false;
				break;
			}
		}
		if(TestWith->GetSpot(TestTetromino.MainPiece.Position.x, TestTetromino.MainPiece.Position.y).Active) {
			Current_Succes = false;
		}

		if(Current_Succes) {
			Succesful_Translation = i;
			Succes_Final = true;
			break;
		}
	}

	if(Succes_Final) {
		Tetromino Temp;
		Temp = Rotate(Dir);
		for(int i=0; i<Length; i++) {
			Pieces[i] = Temp.Pieces[i];
		}
		MainPiece.Position.x += Translations[Succesful_Translation].x;
		MainPiece.Position.y += Translations[Succesful_Translation].y;
		Rotation = NewRotation;
	}
}

bool Tetromino::MoveDown(World* TestWith) {
	bool Succes = true;

	for(int i=0; i<Length; i++) {
		if(TestWith->GetSpot(Pieces[i].Position.x + MainPiece.Position.x, Pieces[i].Position.y + MainPiece.Position.y - 1).Active) {
			Succes = false;
		}
	}
	if(TestWith->GetSpot(MainPiece.Position.x, MainPiece.Position.y - 1).Active) {
		Succes = false;
	}

	if(Succes) {
		MainPiece.Position.y--;
	}
	else {
		TestWith->AbsorbTetromino(*this);
	}
	return(!Succes);
}

void Tetromino::MoveSide(int Side, World* TestWith) {
	bool Succes = true;

	for(int i=0; i<Length; i++) {
		if(TestWith->GetSpot(Pieces[i].Position.x + MainPiece.Position.x + Side, Pieces[i].Position.y + MainPiece.Position.y).Active) {
			Succes = false;
		}
	}
	if(TestWith->GetSpot(MainPiece.Position.x + Side, MainPiece.Position.y).Active) {
		Succes = false;
	}

	if(Succes) {
		MainPiece.Position.x += Side;
	}
}

int World::CheckLines(std::unique_ptr<int[]>* ReturnList) {
	*ReturnList = std::make_unique<int[]>(y);
	int Amount=0;
	for(int i=0; i<y; i++) {
		int Count=0;
		for(int k=0; k<x; k++) {
			if(Info[i][k].Active) {
				Count++;
			}
		}
		if(Count == x) {
			(*ReturnList)[Amount] = i;
			Amount++;
		}
	}
	return Amount;
}

void World::ClearLine(int Line) {
	for(int i=Line; i<(y-1); i++) {
		Info[i] = std::move(Info[i+1]);
		for(int k=0; k<x; k++) {
			Info[i][k].Position.y--;
		}
	}
	Info[y-1] = std::make_unique<Mino[]>(x);
	for(int i=0; i<x; i++) {
		Info[y-1][i].Position.x = i;
		Info[y-1][i].Position.y = y-1;
		Info[y-1][i].Active = false;
	}
}

class RepeatingKeyPress {
	double TimeSinceHold, AutoRepeat_Speed, AutoRepeat_Delay;
	bool BeingHeld;
	public:
	RepeatingKeyPress() {
		TimeSinceHold = 0;
		AutoRepeat_Speed = 0;
		AutoRepeat_Delay = 0;
		BeingHeld = false;
	}
	void Press() {
		BeingHeld = true;
	}
	void UnPress() {
		BeingHeld = false;
		TimeSinceHold = 0;
	}
	void SetValues(double AutoRepeat_SpeedSet, double AutoRepeat_DelaySet) {
		AutoRepeat_Speed = AutoRepeat_SpeedSet;
		AutoRepeat_Delay = AutoRepeat_DelaySet;
	}
	bool Tick(double Time) {
		if(BeingHeld) {
			TimeSinceHold += Time;
			if(TimeSinceHold > AutoRepeat_Delay) {
				TimeSinceHold -= AutoRepeat_Speed;
				return true;
			}
		}
	}
	bool IsBeingHeld() {
		return BeingHeld;
	}
};