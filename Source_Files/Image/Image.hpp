#ifndef Image_HPP
#define Image_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

class Image {
	SDL_Texture* ImageFile;
	int Width, Height;
	public:
	void Free();

	Image();

	~Image();

	bool LoadImage(std::string Path, SDL_Renderer* Render);

	bool LoadFromText(std::string Text, TTF_Font* Font, SDL_Renderer* Render, SDL_Color Color);

	void Draw(int x, int y, SDL_Renderer* Render, SDL_Rect* clip = nullptr, double Angle = 0.0, SDL_Point* Center = nullptr, SDL_RendererFlip Flip = SDL_FLIP_NONE);

	void SetColor(long r, long g, long b);

	void SetAlpha(long a);
	
	void SetBlendMode(SDL_BlendMode BlendMode);
};

#endif