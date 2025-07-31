#include "MathVector.h"
#include "RenderObject.h"
#include "Node.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>

void Node::render(SDL_Renderer* r)
{
	return;
}

void Device::setOffsets()
{
	return;
}

Device::Device(MVector _pos)
{
	this->setOffsets();
	position = _pos;
	for (int connectionIndex = 0; connectionIndex < offsets.size(); connectionIndex++)
	{
		Node* newNode = new Node(_pos + offsets[connectionIndex]);
		Terminal* newTerm = new Terminal(this, newNode, connectionIndex, _pos + offsets[connectionIndex]);
		terminals.push_back(newTerm);
	}
}

Node* mergeNodes(Node* A, Node* B)
{
	Node* C = new Node((A->getPosition() + B->getPosition()) * 0.5);

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
	{
		double radius = 10;
		std::vector<SDL_FPoint> points;

		for (int x = -radius; x < radius; x++)
		{
			for (int y = -radius; y < radius; y++)
			{
				if (x * x + y * y < radius * radius)
				{
					if ((position[0] + x) > 0 && (position[1] + y) > 0) {
						points.push_back({ (float)(position[0] + x), (float)(position[1] + y) });
					}
				}
			}
		}
		SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
		SDL_RenderPoints(r, points.data(), points.size());
	}
}