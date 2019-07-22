#include "Mino.hpp"


void Mino::SetImage(Image* LoadImage) {
	ImageFile = LoadImage;
}

void Mino::SetImageFromType(std::unique_ptr<Image[]>* ImageList, Image* InactiveImage) {
	if(Active) {
		ImageFile = &((*ImageList)[Type]);
	}
	else {
		ImageFile = InactiveImage;
	}
}

SDL_Point& Mino::GetPosition() {
	return Position;
}

void Mino::SetPosition(int x, int y) {
	Position.x = x;
	Position.y = y;
}

void Mino::SetType(int NewType) {
	Type = NewType;
}

void Mino::Draw(SDL_Renderer* Render, int X_Offset, int Y_Offset) {
	ImageFile->Draw(X_Offset + 28 * Position.x, 720 - (Y_Offset + 28 * Position.y), Render);
}

bool& Mino::GetActive() {
	return Active;
}