#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <iostream>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include "MathVector.h"

using namespace std;

void process(bool& running, SDL_Renderer* r)
{
	SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
	SDL_RenderLine(r, 0, 0, 500, 500);
	SDL_FRect rect = SDL_FRect({ 100, 100, 200, 300 });
	SDL_RenderRect(r, &rect);

	return;
}

int main(void)
{
	MVector a(3, 0.0, 0.7, 10.0);
	MVector b(3, 0.1, 0.8, 0.95);

	cout << a[1] << endl;

	MVector c = a + b;

	c.print();


	SDL_Init(SDL_INIT_VIDEO);
	bool global_running = true;

	SDL_Window* window = SDL_CreateWindow("Circuit Simulator 2023", 1280, 720, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	while (global_running)
	{
		SDL_Event events;

		if (SDL_WaitEvent(&events))
		{
			if (events.type == SDL_EVENT_QUIT) { global_running = false; }
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_RenderClear(renderer);
		
		process(global_running, renderer);

		SDL_RenderPresent(renderer);
	}
	SDL_Quit();

	return 0;
}