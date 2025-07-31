#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <iostream>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include "MathVector.h"
#include "Node.h"
#include "Resistor.h"
#include "VoltageSource.h"
#include "vector"

using namespace std;

vector<Device*> devices;

void process(bool& running, SDL_Renderer* r)
{
	for (int nodeIndex = 0; nodeIndex < devices.size(); nodeIndex++)
	{
		devices[0]->render(r);
	}

	return;
}

int main(void)
{
	
	Ground* g = new Ground(MVector(2, 100.0, 100.0));

	devices.push_back((Device*)g);

	SDL_Init(SDL_INIT_VIDEO);
	bool global_running = true;

	SDL_Window* window = SDL_CreateWindow("Circuit Engine 2025", 1280, 720, 0);
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
	
	devices.clear();

	return 0;
}