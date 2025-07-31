#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>

class RenderObject
{
public:
	virtual void render(SDL_Renderer* r) { return; };
};