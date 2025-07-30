#include <SDL3/SDL.h>


void process(bool& running, SDL_Renderer* r)
{

}

int main(void)
{
	SDL_Init(SDL_INIT_VIDEO);
	bool global_running = true;

	SDL_Window* window = SDL_CreateWindow("Circuit Simulator 2023", 1280, 720, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, "rend");
	while (global_running)
	{
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);
		
		process(global_running, renderer);

		SDL_RenderPresent(renderer);
	}
	SDL_Quit();

	return 0;
}