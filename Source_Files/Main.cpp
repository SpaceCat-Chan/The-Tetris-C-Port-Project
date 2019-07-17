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
	TOTAL_MODES
};

enum Rotations {
	ORIGINAL,
	RIGHT,
	DOUBLE,
	LEFT,
	TOTAL_ROTATIONS
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
	friend class World;
	int Type=0;
	Image* ImageFile=nullptr;
	SDL_Point Position;
	bool Active = true;
	public:

	Mino() {
		Position.x = -1;
		Position.y = -1;
	}

	Mino(int x, int y) {
		Position.x = x;
		Position.y = y;
	}

	~Mino() = default;

	Mino(Mino& Copy) = default;

	Mino& operator=(Mino& Copy) = default;

	Mino(Mino&& Move) = default;

	Mino& operator=(Mino&& Move) = default;

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
	bool GetActive() {
		return Active;
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

class World;

class Tetromino {
	friend class World;
	Mino MainPiece, *Pieces=nullptr;
	int Type, Length, Rotation;
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

	void ResetShape(int Length, int Mode);

	void SetLocation(int x, int y) {
		MainPiece.SetPosition(x, y);
	}
	Tetromino Rotate(bool Dir);

	void RotateSelf(bool Dir, World* TestWith);

	bool MoveDown(World* Testwith);

	void SetImages(Image** ImageList) {
		for(int i=0; i<Length; i++) {
			Pieces[i].SetImageFromType(ImageList);
		}
		MainPiece.SetImageFromType(ImageList);
	}
};

class World {
	Mino **Info=nullptr, Random{-1, -1};
	int x,y;
	public:
	World() {
		Info = new Mino*[25];
		for(int i=0; i<25; i++) {
			Info[i] = new Mino[10];
			for(int k=0; k<10; k++) {
				Info[i][k].Active = false;
			}
		}
		x = 10;
		y = 25;
	}
	World(int X, int Y) {
		Info = new Mino*[Y];
		for(int i=0; i<Y; i++) {
			Info[i] = new Mino[X];
			for(int k=0; k<X; k++) {
				Info[i][k].Active = false;
			}
		}
		x = X;
		y = Y;
	}
	Mino& GetSpot(int X, int Y) {
		if(X < 0 || X >= x || Y < 0 || Y>= y) {
			return Random;
		}
		return Info[Y][X];
	}
	void AbsorbTertomino(Tetromino& Absorb) {
		for(int i=0; i<Absorb.Length; i++) {
			Info[Absorb.Pieces[i].GetPosition().y][Absorb.Pieces[i].GetPosition().x] = Absorb.Pieces[i];
		}
		Info[Absorb.MainPiece.GetPosition().y][Absorb.MainPiece.GetPosition().x];

		delete[] Absorb.Pieces;
		Absorb.Pieces = nullptr;
		Mino Random;
		Absorb.MainPiece = Random;
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

	std::uniform_int_distribution<int> dist(0,6);
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
	Rotation = Rotations::ORIGINAL;
}

Tetromino Tetromino::Rotate(bool Dir) {
	Tetromino Rotated;
	Rotated = *this;

	for(int i=0; i<Length; i++) {
		Rotated.Pieces[i].Position = RotatePoint(Dir, (Rotated.Pieces[i].Position));
	}
	return Rotated;
}

SDL_Point* SubtractPointLists(SDL_Point* A, SDL_Point* B) {
	SDL_Point* Result;
	Result = new SDL_Point[5];

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

	SDL_Point** Final_Offset;

	if(Type == TetrominoList::I) {
		Final_Offset = I_OffSet;
	}
	else if(Type == TetrominoList::O) {
		Final_Offset = O_OffSet;
	}
	else {
		Final_Offset = Other_OffSet;
	}

	int NewRotation;
	
	NewRotation = (Dir) ? (Rotation + 1) : (Rotation - 1);

	if(NewRotation < Rotations::ORIGINAL) {
		NewRotation = Rotations::LEFT;
	}
	else if(NewRotation > Rotations::LEFT) {
		NewRotation = Rotations::ORIGINAL;
	}

	SDL_Point* Translations;
	Translations = SubtractPointLists(Final_Offset[Rotation], Final_Offset[NewRotation]);

	bool Succes_Final = false;
	int Succesful_Translation = 0;

	for(int i=0; i<5; i++) {
		Tetromino TestTetromino;
		bool Current_Succes = true;

		TestTetromino = Rotate(Dir);
		TestTetromino.MainPiece.SetPosition(TestTetromino.MainPiece.GetPosition().x + Translations[i].x, TestTetromino.MainPiece.GetPosition().y + Translations[i].y);
		for(int k=0; k<TestTetromino.Length; k++) {
			if(TestWith->GetSpot(TestTetromino.Pieces[i].Position.x + TestTetromino.MainPiece.Position.x, TestTetromino.Pieces[i].Position.y + TestTetromino.MainPiece.Position.y).Active) {
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
		*this = Rotate(Dir);
		MainPiece.Position.x += Translations[Succesful_Translation].x;
		MainPiece.Position.y += Translations[Succesful_Translation].y;
	}
}

bool Tetromino::MoveDown(World* TestWith) {
	bool Succes = true;

	for(int i=0; i<Length; i++) {
		if(TestWith->GetSpot(Pieces[i].Position.x + MainPiece.Position.x, Pieces[i].Position.y + MainPiece.Position.y + 1).Active) {
			Succes = false;
		}
	}
	if(TestWith->GetSpot(MainPiece.Position.x, MainPiece.Position.y + 1).Active) {
		Succes = false;
	}

	if(Succes) {
		MainPiece.Position.y++;
	}

	return(!Succes);
}