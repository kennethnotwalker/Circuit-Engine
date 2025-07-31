#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <stdio.h>
#include <iostream>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_oldnames.h>
#include "MathVector.h"
#include "Node.h"
#include "vector"
#include "SDL3_ttf/SDL_ttf.h"

using namespace std;

vector<Device*> devices;
vector<Terminal*> forced;
const double STEP_SIZE = 0.1;

void process(bool& running, SDL_Renderer* r)
{
	
	return;
}

int main(void)
{
	Device* g = new Device(MVector(2, 400.0, 400.0), 0, 1);
	Device* v = new Device(MVector(2, 400.0, 300.0), 2, 2); v->value = 10;
	Device* r = new Device(MVector(2, 600.0, 300.0), 1, 2); r->value = 10;
	Device* r2 = new Device(MVector(2, 800.0, 300.0), 1, 2); r2->value = 10;

	connectTerminals(g->getTerminal(0), v->getTerminal(0));
	connectTerminals(v->getTerminal(1), r->getTerminal(0));
	connectTerminals(r->getTerminal(1), r2->getTerminal(0));
	connectTerminals(r2->getTerminal(1), g->getTerminal(0));

	devices.push_back((Device*)g);
	devices.push_back((Device*)v);
	devices.push_back((Device*)r);
	devices.push_back((Device*)r2);

	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
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