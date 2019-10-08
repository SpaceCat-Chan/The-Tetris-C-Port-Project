#include "World.hpp"

#define Puyo_Types 5
#define ChainLength 7
#define Max_Types_In_One_Piece 2

int World::CheckLines(std::unique_ptr<int[]>& ReturnList) {
	ReturnList = std::make_unique<int[]>(y);
	int Amount=0;
	for(int i=0; i<y; i++) {
		int Count=0;
		for(int k=0; k<x; k++) {
			if(Info[i][k].GetActive()) {
				Count++;
			}
		}
		if(Count == x) {
			ReturnList[Amount] = i;
			Amount++;
		}
	}
	return Amount;
}



Mino& World::GetSpot(int X, int Y) {
	if(X < 0 || X >= x || Y < 0 || Y>= y) {
		return Random;
	}
	return Info[Y][X];
}

void World::AbsorbTetromino(Tetromino& Absorb) {
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

void World::SetImages(std::unique_ptr<Image[]>* ImageList, Image* InactiveImage) {
	for(int i=0; i<y; i++) {
		for(int k=0; k<x; k++) {
			Info[i][k].SetImageFromType(ImageList, InactiveImage);
		}
	}
}

void World::Draw(SDL_Renderer* Render, int X, int Y) {
	for(int i=0; i<y; i++) {
		for(int k=0; k<x; k++) {
			Info[i][k].Draw(Render, X, Y+28);
		}
	}
}

void World::ClearLine(int Line) {
	for(int i=Line; i<(y-1); i++) {
		Info[i] = std::move(Info[i+1]);
		for(int k=0; k<x; k++) {
			Info[i][k].GetPosition().y--;
		}
	}
	Info[y-1] = std::make_unique<Mino[]>(x);
	for(int i=0; i<x; i++) {
		Info[y-1][i].GetPosition().x = i;
		Info[y-1][i].GetPosition().y = y-1;
		Info[y-1][i].GetActive() = false;
	}
}

void World::Reset() {
	Info = std::make_unique<std::unique_ptr<Mino[]>[]>(y);
	for(int i=0; i<y; i++) {
		Info[i] = std::make_unique<Mino[]>(x);
		for(int k=0; k<x; k++) {
			Info[i][k].GetActive() = false;
			Info[i][k].GetPosition().x = k;
			Info[i][k].GetPosition().y = i;
		}
	}
	Random.GetActive() = true;
}

int World::LinesAbove(int LineNumber /* inclusive*/) {
	int count=0;
	for(int i = LineNumber; i < y; i++) {
		for(int k = 0; k < x; k++) {
			if(Info[i][k].Active == true) {
				count++;
			}
		}
	}
	return count;
}

void Tetromino::ResetShape(std::mt19937& Engine, int length/*=3*/, int Mode/*=Modes::Standard*/) {
	if(length == -1) {
		std::mt19937 Engine;
		ResetShape(Engine, 0);
		Rotation = -1; //validity checking, use GetRotation() to check if the Tetromino is Valid
		return;
	}
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

	SDL_Point* Selected_Template;
	int RandomNum;
	RandomNum = dist(Engine);


	if(RandomNum == 0) {
		Selected_Template = S_template;
	}
	else if(RandomNum == 1) {
		Selected_Template = Z_template;
	}
	else if(RandomNum == 2) {
		Selected_Template = L_template;
	}
	else if(RandomNum == 3) {
		Selected_Template = J_template;
	}
	else if(RandomNum == 4) {
		Selected_Template = I_template;
	}
	else if(RandomNum == 5) {
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

	if(Mode == 3) {
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
	Rotation = 0;
}

void Tetromino::ResetWithUpcomming(std::mt19937& Engine, Tetromino& Upcoming, int Length/*=3*/, int Mode/*=0*/) {
	do{
		*this = Upcoming;
		Upcoming.ResetShape(Engine, Length, Mode);
	}
	while(Rotation == -1);
}

void Tetromino::SetLocation(int x, int y) {
	MainPiece.SetPosition(x, y);
}

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

Tetromino Tetromino::Rotate(bool Dir) {
	Tetromino Rotated;
	Rotated = (*this);

	for(int i=0; i<Length; i++) {
		Rotated.Pieces[i].GetPosition() = RotatePoint(Dir, (Rotated.Pieces[i].GetPosition()));
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

	if(Type == 4) {
		Final_Offset = std::move(I_OffSet);
	}
	else if(Type == 6) {
		Final_Offset = std::move(O_OffSet);
	}
	else {
		Final_Offset = std::move(Other_OffSet);
	}

	int NewRotation;
	
	NewRotation = (!Dir) ? (Rotation + 1) : (Rotation - 1);

	if(NewRotation < 0) {
		NewRotation = 3;
	}
	else if(NewRotation > 3) {
		NewRotation = 0;
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
			if(TestWith->GetSpot(TestTetromino.Pieces[k].GetPosition().x + TestTetromino.MainPiece.GetPosition().x, TestTetromino.Pieces[k].GetPosition().y + TestTetromino.MainPiece.GetPosition().y).GetActive()) {
				Current_Succes = false;
				break;
			}
		}
		if(TestWith->GetSpot(TestTetromino.MainPiece.GetPosition().x, TestTetromino.MainPiece.GetPosition().y).GetActive()) {
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
		MainPiece.GetPosition().x += Translations[Succesful_Translation].x;
		MainPiece.GetPosition().y += Translations[Succesful_Translation].y;
		Rotation = NewRotation;
	}
}

bool Tetromino::MoveDown(World* TestWith, bool InhibitAbsorb/*=false*/) {
	bool Succes = true;

	for(int i=0; i<Length; i++) {
		if(TestWith->GetSpot(Pieces[i].GetPosition().x + MainPiece.GetPosition().x, Pieces[i].GetPosition().y + MainPiece.GetPosition().y - 1).GetActive()) {
			Succes = false;
		}
	}
	if(TestWith->GetSpot(MainPiece.GetPosition().x, MainPiece.GetPosition().y - 1).GetActive()) {
		Succes = false;
	}

	if(Succes) {
		MainPiece.GetPosition().y--;
	}
	else {
		if(!InhibitAbsorb) {
			TestWith->AbsorbTetromino(*this);
		}
	}
	return(!Succes);
}

void Tetromino::SetImages(std::unique_ptr<Image[]>* ImageList) {
	for(int i=0; i<Length; i++) {
		Pieces[i].SetImageFromType(ImageList);
	}
	MainPiece.SetImageFromType(ImageList);
}

void Tetromino::Draw(SDL_Renderer* Render, int X, int Y) {
	for(int i=0; i<Length; i++) {
		Pieces[i].Draw(Render, MainPiece.GetPosition().x * 28 + X, MainPiece.GetPosition().y * 28 + Y + 28);
	}
	MainPiece.Draw(Render, X, Y+28);
}

void Tetromino::MoveSide(int Side, World* TestWith) {
	bool Succes = true;

	for(int i=0; i<Length; i++) {
		if(TestWith->GetSpot(Pieces[i].GetPosition().x + MainPiece.GetPosition().x + Side, Pieces[i].GetPosition().y + MainPiece.GetPosition().y).GetActive()) {
			Succes = false;
		}
	}
	if(TestWith->GetSpot(MainPiece.GetPosition().x + Side, MainPiece.GetPosition().y).GetActive()) {
		Succes = false;
	}

	if(Succes) {
		MainPiece.GetPosition().x += Side;
	}
}

Tetromino Tetromino::MakeGhost(World& TestWith, std::unique_ptr<Image[]>* ImageList) {
	Tetromino Temp;
	Temp = *this;
	Temp.SetImages(ImageList);
	bool Result;
	do{
		Result = Temp.MoveDown(&TestWith, true);
	}
	while(!Result);
	return Temp;
}