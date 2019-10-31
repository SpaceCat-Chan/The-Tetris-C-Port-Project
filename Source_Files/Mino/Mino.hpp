#ifndef Mino_HPP
#define Mino_HPP something

#include <SDL.h>
#include <memory>
#include "../../SDL-Helper-Libraries/Image/Image.hpp"

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

	void SetImage(Image* LoadImage);

	void SetImageFromType(std::unique_ptr<Image[]>* ImageList, Image* InactiveImage = nullptr);

	SDL_Point& GetPosition();

	void SetPosition(int x, int y);

	void SetType(int NewType);

	void Draw(SDL_Renderer* Render, int X_Offset, int Y_Offset);

	bool& GetActive();
};

#endif