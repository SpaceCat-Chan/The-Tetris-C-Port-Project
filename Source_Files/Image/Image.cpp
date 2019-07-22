#include "Image.hpp"

void Image::Free() {
	if(ImageFile == nullptr) {
		SDL_free(ImageFile);
		ImageFile = nullptr;
		Width = 0;
		Height = 0;
	}
}

Image::Image() {
	Height = 0;
	Width = 0;
	ImageFile = nullptr;
}

Image::~Image() {
	Free();
}

bool Image::LoadImage(std::string Path, SDL_Renderer* Render) {
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

void Image::Draw(int x, int y, SDL_Renderer* Render, SDL_Rect* clip, double Angle, SDL_Point* Center, SDL_RendererFlip Flip) {
	//Set rendering space and render to screen
	if(this) {
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
	else {
		std::cout << "ERROR, this = nullptr, unable to draw\n";
	}
}

void Image::SetColor(long r, long g, long b) {
	SDL_SetTextureColorMod(ImageFile, r, g, b);
}

void Image::SetAlpha(long a) {
	SDL_SetTextureAlphaMod(ImageFile, a);
}

void Image::SetBlendMode(SDL_BlendMode BlendMode) {
	SDL_SetTextureBlendMode(ImageFile, BlendMode);
}