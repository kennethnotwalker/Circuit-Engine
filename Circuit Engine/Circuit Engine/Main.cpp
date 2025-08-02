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
#include "ImageLoader.h"
#include "Complex.h"

using namespace std;

ImageLoader imageLoader;

vector<Device*> devices;
vector<Wire*> wires;
vector<Terminal*> forced;
bool simulating = false;
const double STEP_SIZE = 0.1;

bool KEYSDOWN[322];
bool KEYSUP[322];
bool KEYSHELD[322];

bool mouseDown, mouseJustPressed, mouseJustReleased = false;

MVector heldPosition = MVector(2, 0, 0);

void process(bool& running, SDL_Renderer* r)
{
	if (KEYSDOWN[SDL_Scancode::SDL_SCANCODE_SPACE])
	{
		simulating = !simulating;
	}
	float mouseX, mouseY = 0;

	SDL_GetMouseState(&mouseX, &mouseY);

	MVector mousePosition(2, mouseX, mouseY);
	if (mouseJustPressed)
	{
		heldPosition = mousePosition;
	}
	if (mouseDown)
	{
		SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
		SDL_FRect rect = { mouseX - 5, mouseY - 5, 10, 10 };

		MVector projectedPos = Wire::snap(heldPosition, mousePosition);
		SDL_RenderLine(r, heldPosition[0], heldPosition[1], projectedPos[0], projectedPos[1]);
	}
	if (mouseJustReleased)
	{
		Wire* wire = new Wire(heldPosition, Wire::snap(heldPosition, mousePosition));
		if (wire->connected)
		{
			wires.push_back(wire);
		}
		else
		{
			delete wire;
		}
	}

	std::vector<Node*> nodes = getNodeList();
	if (simulating) {
		ComplexMatrix* solver = new ComplexMatrix(nodes.size(), nodes.size() + 1);

		vector<int> equationIDs;
		int equationsInSolver = 0;

		for (int index = 0; index < nodes.size(); index++)
		{
			Node* node = nodes[index];
			std::vector<complex*> equations;
			bool forced = false;

			node->generateEquations(solver, equations, equationIDs, forced);

			for (int i = 0; i < equations.size(); i++)
			{
				if (equationsInSolver < solver->rows && solver->addLIRow(equationsInSolver, equations[i]))
				{
					equationsInSolver++;
				}
				delete[] equations[i];
			}

		}

		solver->print();

		ComplexMatrix* solution = solver->RREF();
		cout << "solved: " << endl;
		solution->print();

		for (int i = 0; i < solution->rows; i++)
		{
			int nonzeroes = 0;
			int solutionIndex = 0;

			for (int c = 0; c < solution->rows; c++)
			{
				if (abs(solution->get(i, c)) > 0.000001)
				{
					nonzeroes++;
				}
				if (abs(solution->get(i, c) - 1) < 0.00001)
				{
					solutionIndex = c;
				}
			}
			if (nonzeroes == 1)
			{
				//cout << "Row " << i << ": Setting Node " << solutionIndex << " to " << solution->get(i, solution->cols - 1) << endl;
				getNodeByID(solutionIndex)->voltage = solution->get(i, solution->cols - 1);
			}
		}

		delete solution;
		delete solver;
	}

	for (int d = 0; d < devices.size(); d++)
	{
		devices[d]->render(r);
	}

	for (int n = 0; n < nodes.size(); n++)
	{
		Node* node = nodes[n];
		node->render(r);
	}

	
	return;
}

int main(void)
{
	for (int i = 0; i < 322; i++)
	{
		KEYSDOWN[i] = false;
		KEYSUP[i] = false;
		KEYSHELD[i] = false;
	}

	complex a = 3 + 5*_i;

	cout << a << endl;


	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	bool global_running = true;

	SDL_Window* window = SDL_CreateWindow("Circuit Engine 2025", 1280, 720, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	//load images
	imageLoader.loadImage(renderer, "symbol_R", "imgs/resistor.png");
	imageLoader.loadImage(renderer, "symbol_V", "imgs/vsource.png");
	imageLoader.loadImage(renderer, "symbol_G", "imgs/ground.png");

	//load devices
	Device* g = new Device(MVector(2, 640.0, 400.0), 0, 1, "symbol_G", imageLoader); g->rotation = 180;
	Device* v = new Device(g->position + MVector(2, 0, -100.0), 2, 2, "symbol_V", imageLoader); v->setProperty("voltage", 10 + 7 * _i);
	Device* r = new Device(v->position + MVector(2, 50.0, -50.0), 1, 2, "symbol_R", imageLoader); r->rotation = 90; r->setProperty("resistance", 5);
	Device* r2 = new Device(r->position + MVector(2, 100.0, 0.0), 1, 2, "symbol_R", imageLoader); r2->rotation = 90; r2->setProperty("resistance", 10);
	Device* r3 = new Device(r->position + MVector(2, 50.0, -50.0), 1, 2, "symbol_R", imageLoader); r3->rotation = 0; r3->setProperty("resistance", 15);
	Device* g2 = new Device(r3->position + MVector(2, 0.0, -100.0), 0, 1, "symbol_G", imageLoader); g2->rotation = 0;
	Device* v2 = new Device(r2->position + MVector(2, 50.0, 50.0), 2, 2, "symbol_V", imageLoader); v2->setProperty("voltage", 15);

	connectJunction(g->terminals[0], v->terminals[0]);
	connectJunction(v->terminals[1], r->terminals[0]);
	connectJunction(r->terminals[1], r2->terminals[0]);
	connectJunction(r->terminals[1], r3->terminals[0]);
	connectJunction(r3->terminals[1], g2->terminals[0]);
	connectJunction(r2->terminals[1], v2->terminals[1]);
	connectJunction(v2->terminals[0], g->terminals[0]);

	devices.push_back((Device*)g);
	devices.push_back((Device*)g2);
	devices.push_back((Device*)v);
	devices.push_back((Device*)v2);
	devices.push_back((Device*)r);
	devices.push_back((Device*)r2);
	devices.push_back((Device*)r3);
	

	while (global_running)
	{
		for (int i = 0; i < 322; i++)
		{
			KEYSDOWN[i] = false;
			KEYSUP[i] = false;
		}

		mouseJustPressed = false;
		mouseJustReleased = false;
		SDL_Event events;
		while(SDL_PollEvent(&events))
		{
			switch (events.type)
			{
				case SDL_EVENT_QUIT:
					global_running = false;
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					mouseJustPressed = true;
					mouseDown = true;
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					if (!mouseJustPressed)
					{
						mouseJustReleased = true;
						mouseDown = false;
					}
					break;
				case SDL_EVENT_KEY_DOWN:
					if (!KEYSHELD[events.key.scancode]) { KEYSDOWN[events.key.scancode] = true; }
					KEYSHELD[events.key.scancode] = true;
					break;
				case SDL_EVENT_KEY_UP:
					if (KEYSHELD[events.key.scancode]) { KEYSUP[events.key.scancode] = true; }
					KEYSHELD[events.key.scancode] = false;
					break;
			}
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