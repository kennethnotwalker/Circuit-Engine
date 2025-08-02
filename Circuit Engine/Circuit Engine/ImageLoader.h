#pragma once
#include <map>
#include <string>
#include <SDL3_image/SDL_image.h>
using namespace std;

class ImageLoader
{
	map<string, SDL_Surface*> surfaceCache;
	map<string, SDL_Texture*> textureCache;
	
public:
	ImageLoader() {};
	void loadImage(SDL_Renderer* renderer, string alias, string path);
	SDL_Texture* getImage(string alias);
};