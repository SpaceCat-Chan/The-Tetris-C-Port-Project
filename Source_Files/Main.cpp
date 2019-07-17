#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <random>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define yeet return 1

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
	TOTAL_AMOUNT
};

class Image {
	SDL_Texture* ImageFile=nullptr;
	int Width, Height;
	public:
	void Free() {
		if(ImageFile != nullptr) {
			SDL_free(ImageFile);
			ImageFile = nullptr;
			Width = 0;
			Height = 0;
		}
	}
	Image() {
		Height = 0;
		Width = 0;
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
	int Type=0;
	Image* ImageFile=nullptr;
	SDL_Point Position;
	public:
	void SetImage(Image* LoadImage) {
		ImageFile = LoadImage;
	}
	void SetImageFromType(Image** ImageList) {
		ImageFile = ImageList[Type];
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
};

SDL_Point RotatePoint(bool Dir, SDL_Point ToRotate) { //Dir: True = Clockwise
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

class Tetromino {
	Mino MainPiece, *Pieces=nullptr;
	int Type, Length;
	public:
	
	Tetromino() {}

	~Tetromino() {
		if(Pieces != nullptr) {
			delete[] Pieces;
		}
	}

	Tetromino(Tetromino& Copy) {
		if(Copy.Pieces != nullptr) {
			MainPiece = Copy.MainPiece;
			Type = Copy.Type;
			Length = Copy.Length;
			Pieces = new Mino[Length];

			for(int i=0; i<Length; i++) {
				Pieces[i] = Copy.Pieces[i];
			}
		}
	}

	Tetromino& operator= (Tetromino& Copy) {
		MainPiece = Copy.MainPiece;
		Type = Copy.Type;
		Length = Copy.Length;
		delete[] Pieces;
		Pieces = new Mino[Length];

		for(int i=0; i<Length; i++) {
			Pieces[i] = Copy.Pieces[i];
		}
		return *this;
	}

	Tetromino(Tetromino&& Move) {
		MainPiece = Move.MainPiece;
		Type = Move.Type;
		Length = Move.Length;
		Pieces = Move.Pieces;
	}

	Tetromino& operator= (Tetromino&& Move) {
		MainPiece = Move.MainPiece;
		Type = Move.Type;
		Length = Move.Length;
		delete[] Pieces;
		Pieces = Move.Pieces;
		return *this;
	}

	void ResetShape(int Length=3, int Mode=Modes::Standard);

	void SetLocation(int x, int y) {
		MainPiece.SetPosition(x, y);
	}
	Tetromino Rotate(bool Dir);

	void SetImages(Image** ImageList) {
		for(int i=0; i<Length; i++) {
			Pieces[i].SetImageFromType(ImageList);
		}
		MainPiece.SetImageFromType(ImageList);
	}
};

bool init(SDL_Window** window, SDL_Renderer** Render);

void close(SDL_Window* window, SDL_Renderer* Render);

int main(int argc, char* arg[]) {
	SDL_Window* Window=nullptr;
	SDL_Renderer* Render=nullptr;

	if(init(&Window, &Render)) {
		bool Quit=false;

		SDL_Event Event_Handler;

		Image* TetrominoImages[TetrominoList::TOTAL_AMOUNT];

		TetrominoImages[TetrominoList::S]->LoadImage("Minos/SPiece.png", Render);
		TetrominoImages[TetrominoList::Z]->LoadImage("Minos/ZPiece.png", Render);
		TetrominoImages[TetrominoList::L]->LoadImage("Minos/LPiece.png", Render);
		TetrominoImages[TetrominoList::J]->LoadImage("Minos/LPiece.png", Render);
		TetrominoImages[TetrominoList::I]->LoadImage("Minos/IPiece.png", Render);
		TetrominoImages[TetrominoList::T]->LoadImage("Minos/TPiece.png", Render);
		TetrominoImages[TetrominoList::O]->LoadImage("Minos/OPiece.png", Render);

		Image* GhostImages[TetrominoList::TOTAL_AMOUNT];

		GhostImages[TetrominoList::S]->LoadImage("Minos/SPieceGhost.png", Render);
		GhostImages[TetrominoList::Z]->LoadImage("Minos/ZPieceGhost.png", Render);
		GhostImages[TetrominoList::L]->LoadImage("Minos/LPieceGhost.png", Render);
		GhostImages[TetrominoList::J]->LoadImage("Minos/LPieceGhost.png", Render);
		GhostImages[TetrominoList::I]->LoadImage("Minos/IPieceGhost.png", Render);
		GhostImages[TetrominoList::T]->LoadImage("Minos/TPieceGhost.png", Render);
		GhostImages[TetrominoList::O]->LoadImage("Minos/OPieceGhost.png", Render);

		Image Blank;
		Blank.LoadImage("Minos/Blank.png", Render);

		Image Background, Title;
		Background.LoadImage("BackGround.png", Render);
		Title.LoadImage("Title.png", Render);

		while(!Quit) {
			while(SDL_PollEvent(&Event_Handler)) {
				if(Event_Handler.type == SDL_QUIT) {
					Quit = true;
				}
			}



		}

	}
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

void Close(SDL_Window* window, SDL_Renderer* Render) {
	SDL_DestroyWindow( window );
	SDL_DestroyRenderer(Render);

	IMG_Quit();

	SDL_Quit();

	std::cout << "exited succesfully\n";
}

 void Tetromino::ResetShape(int length=3, int Mode=Modes::Standard) {
	if(Pieces != nullptr) {
		delete[] Pieces;
		Pieces = nullptr;
	}
	//lot of templates here, just ignore it. i wish there was a way to make all this shorter/take up less space
	SDL_Point S_template[4];
	S_template[0].x = -1;
	S_template[0].y = 0;

	S_template[1].x = 0;
	S_template[1].y = 1;
		
	S_template[2].x = 1;
	S_template[2].y = 1;

	S_template[3].x = 0;
	S_template[3].y = -2;



	SDL_Point Z_template[4];
	Z_template[0].x = 1;
	Z_template[0].y = 0;

	Z_template[1].x = 0;
	Z_template[1].y = 1;
		
	Z_template[2].x = -1;
	Z_template[2].y = 1;

	Z_template[3].x = 1;
	Z_template[3].y = -1;


	SDL_Point L_template[4];
	L_template[0].x = -1;
	L_template[0].y = 0;

	L_template[1].x = 1;
	L_template[1].y = 0;
		
	L_template[2].x = 1;
	L_template[2].y = 1;

	L_template[3].x = -1;
	L_template[3].y = -1;


	SDL_Point J_template[4];
	J_template[0].x = 1;
	J_template[0].y = 0;

	J_template[1].x = -1;
	J_template[1].y = 0;
		
	J_template[2].x = -1;
	J_template[2].y = 1;

	J_template[3].x = 0;
	J_template[3].y = -1;


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
	T_template[2].y = -1;

	T_template[3].x = 0;
	T_template[3].y = -2;


	SDL_Point O_template[4];
	O_template[0].x = 1;
	O_template[0].y = 0;

	O_template[1].x = 0;
	O_template[1].y = 1;
	
	O_template[2].x = 1;
	O_template[2].y = 1;

	O_template[3].x = 0;
	O_template[3].y = -1;

	std::uniform_int_distribution dist(0,6);
	std::mt19937 Engine;

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

	Pieces = new Mino[length];

	for(int i=0; i<length; i++) {
		Pieces[i].SetPosition(Selected_Template[i].x, Selected_Template[i].y);
	}

	Type = RandomNum;

	if(Mode == Modes::Puyo) {
		std::uniform_int_distribution<int> PuyoDist(0, Puyo_Types - 1);

		int* AllowedTypes;

		AllowedTypes = new int[Max_Types_In_One_Piece];

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
}

Tetromino Tetromino::Rotate(bool Dir) {
	Tetromino Rotated;
	Rotated = *this;

	for(int i=0; i<Length; i++) {
		Rotated.Pieces[i].Position = RotatePoint(Dir, (Rotated.Pieces[i].Position));
	}
	return Rotated;
}

