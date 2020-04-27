#pragma once


SDL_Surface* loadSurface(std::string path)
{

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	return loadedSurface;
}

class TextureManager
{
public:
	std::unordered_map<std::string, SDL_Texture*> Textures;
	SDL_Texture *loadTexture(string file)
	{
		auto entry = Textures.find(file);

		if (entry != Textures.end())
		{
			return entry->second;
		}
		SDL_Surface *s = loadSurface(file);
		SDL_Texture *t = SDL_CreateTextureFromSurface(ren, s);
		SDL_FreeSurface(s);
		std::cout << purple_int << "[TexManager] couldn't find " + file + ", loading it" << white << endl;
		Textures.insert(std::make_pair(file, t));

		return t;

	}
};
TextureManager TexManager;

SDL_Texture *bg_texture;
SDL_Surface *bg_surface;

class Sprite
{
public:
	Sprite(std::string sprite, int x, int y, int size_x, int size_y, double angle = 0.0)
	{
		SDL_Rect DestR;

		DestR.x = sp_offset_x + x * scale;
		DestR.y = sp_offset_y + y * scale;
		DestR.w = abs(size_x) * scale;
		DestR.h = abs(size_y) * scale;
		SDL_RendererFlip flip = SDL_FLIP_NONE;
		if (size_x < 0) { flip = SDL_FLIP_HORIZONTAL; }



		SDL_RenderCopyEx(ren, TexManager.loadTexture(sprite), NULL, &DestR, angle, NULL, flip);

	}

};


void RenderBackground(int x, int y)
{
	SDL_Rect SourceR;

	SourceR.x = sp_offset_x + x * scale;
	SourceR.y = sp_offset_y + y * scale;
	SourceR.w = 512*scale;
	SourceR.h = 512*scale;

	SDL_RenderCopyEx(ren, bg_texture, NULL, &SourceR, (double(ASM.Get_Ram(0x36,1))*360.0)/256.0, NULL, SDL_FLIP_NONE);
}