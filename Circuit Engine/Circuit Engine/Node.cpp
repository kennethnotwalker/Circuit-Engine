#include "MathVector.h"
#include "Node.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>


int GLOBAL_ID_COUNTER = 0;
std::vector<Node*> nodeList;

Node::Node()
{
	id = GLOBAL_ID_COUNTER;
	GLOBAL_ID_COUNTER++;
	nodeList.push_back(this);
}

Node::~Node()
{
	
	nodeList.erase(std::remove(nodeList.begin(), nodeList.end(), this), nodeList.end());
	for (int i = 0; i < nodeList.size(); i++)
	{
		if (nodeList[i]->id > id)
		{
			nodeList[i]->id--;
		}
	}
	GLOBAL_ID_COUNTER--;
}

void Device::setOffsets()
{
	return;
}

void Device::stepUpdate(double step)
{
	return;
}

Device::Device(MVector _pos, int type, int terminals)
{
	deviceType = type;
	for (int i = 0; i < terminals; i++)
	{
		offsets.push_back(MVector(2, 0.0, 50.0 * (i - (terminals - 1) / 2.0)));
	}
	init(_pos);
}

void Device::init(MVector _pos)
{
	position = _pos;
	for (int connectionIndex = 0; connectionIndex < offsets.size(); connectionIndex++)
	{
		Node* newNode = new Node();
		Terminal* newTerm = new Terminal(this, newNode, connectionIndex, _pos + offsets[connectionIndex]);
		terminals.push_back(newTerm);
	}
}

Node* mergeNodes(Node* A, Node* B)
{
	Node* C = new Node();

	for (int i = 0; i < A->connected_terminals.size(); i++)
	{
		C->connected_terminals.push_back(A->connected_terminals[i]);
	}
	for (int i = 0; i < B->connected_terminals.size(); i++)
	{
		C->connected_terminals.push_back(B->connected_terminals[i]);
	}

	delete A;
	delete B;

	for (int i = 0; i < C->connected_terminals.size(); i++)
	{
		C->connected_terminals[i]->node = C;
	}

	return C;
}

Node* connectTerminals(Terminal* A, Terminal* B)
{
	return mergeNodes(A->node, B->node);
}

void Device::render(SDL_Renderer* r)
{
	for(int termIndex = 0; termIndex < terminals.size(); termIndex++)
	{
		Terminal* terminal = terminals[termIndex];
		double radius = 10;
		std::vector<SDL_FPoint> points;

		MVector pos = terminal->pos;

		for (int x = -radius; x < radius; x++)
		{
			for (int y = -radius; y < radius; y++)
			{
				if (x * x + y * y < radius * radius)
				{
					if ((pos[0] + x) > 0 && (pos[1] + y) > 0) {
						points.push_back({ (float)(pos[0] + x), (float)(pos[1] + y) });
					}
				}
			}
		}
		SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
		SDL_RenderPoints(r, points.data(), points.size());
		TTF_Font* calibri = TTF_OpenFont("calibri-regular.ttf", 24);

		SDL_Color green = { 0, 255, 0 };
		SDL_Color yellow = { 0, 175, 175 };
		SDL_Color col = green;
		if (terminal->node->forced)
		{
			col = yellow;
		}
		std::string s_voltage = std::to_string(terminal->node->voltage);
		SDL_Surface* textMessage = TTF_RenderText_Solid(calibri, s_voltage.c_str(), s_voltage.size(), green);
		SDL_Texture* tex = SDL_CreateTextureFromSurface(r, textMessage);
		SDL_FRect textRect = { pos[0] - textMessage->w/2, pos[1] - textMessage->h/2 - radius*2, textMessage->w, textMessage->h };
		
		SDL_RenderTexture(r, tex, NULL, &textRect);

		SDL_DestroyTexture(tex);
		SDL_DestroySurface(textMessage);
	}
}

Terminal* Device::getTerminal(int index)
{
	return terminals[index];
}