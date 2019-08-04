#ifndef World_HPP
#define World_HPP Something

#include <SDL2/SDL.h>
#include <memory>
#include "Mino/Mino.hpp"
#include <random>

class Tetromino;

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

	Mino& GetSpot(int X, int Y);

	void AbsorbTetromino(Tetromino& Absorb);

	void SetImages(std::unique_ptr<Image[]>* ImageList, Image* InactiveImage);

	void Draw(SDL_Renderer* Render, int X, int Y);

	//return = size of ReturnList
	int CheckLines(std::unique_ptr<int[]>& ReturnList);

	void ClearLine(int Line);

	void Reset();
};

class Tetromino {
	friend class World;
	Mino MainPiece;
	std::unique_ptr<Mino[]> Pieces;
	int Type, Length, Rotation;
	public:
	
	Tetromino() {
		Rotation = -1; //validity checking, use GetRotation() to check if the Tetromino is Valid
	}

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

	void ResetShape(std::mt19937 Engine, int Length=3, int Mode=0);

	void SetLocation(int x, int y);

	Tetromino Rotate(bool Dir);

	void RotateSelf(bool Dir, World* TestWith);

	bool MoveDown(World* Testwith);

	void SetImages(std::unique_ptr<Image[]>* ImageList);

	void Draw(SDL_Renderer* Render, int X, int Y);

	int GetRotation() {
		return Rotation;
	}

	void MoveSide(int Side, World* TestWith);
};



#endif