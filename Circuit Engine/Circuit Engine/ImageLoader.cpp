#include "ImageLoader.h"
#include <map>
#include <string>
#include <SDL3_image/SDL_image.h>
#include "Constants.h"

using namespace std;

void ImageLoader::loadImage(SDL_Renderer* renderer, string alias, string path)
{
	SDL_Surface* loaded = IMG_Load(path.data());
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, loaded);

	surfaceCache[alias] = loaded;
	textureCache[alias] = tex;
}

SDL_Texture* ImageLoader::getImage(string alias)
{
	return textureCache[alias];
}
