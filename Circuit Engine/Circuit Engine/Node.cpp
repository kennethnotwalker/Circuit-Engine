#include "MathVector.h"
#include "Node.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <numbers>


int GLOBAL_ID_COUNTER = 0;
std::vector<Node*> nodeList;
std::vector<Junction*> junctionList;

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

void Node::render(SDL_Renderer* r)
{
	MVector center = MVector(2, 0, 0);
	int avgCount = 0;
	for (int termIndex = 0; termIndex < junctions.size(); termIndex++) {

		Terminal* other = (Terminal*)junctions[termIndex];
		
		center = center + other->getGlobalPosition();
		avgCount++;
	}
	if (avgCount > 0)
	{
		center = center * (1.0 / avgCount);
		SDL_SetRenderDrawColor(r, 0, 255, 255, 255);
		for (int termIndex = 0; termIndex < junctions.size(); termIndex++) {

			Terminal* other = (Terminal*)junctions[termIndex];

			MVector pos = other->getGlobalPosition();

			SDL_RenderLine(r, center[0], center[1], pos[0], pos[1]);
		}
		TTF_Font* calibri = TTF_OpenFont("calibri-regular.ttf", 24);

		SDL_Color green = { 0, 255, 0 };

		std::string s_id = std::to_string(id);

		SDL_Surface* textMessage = TTF_RenderText_Solid(calibri, s_id.c_str(), s_id.size(), green);
		SDL_Texture* tex = SDL_CreateTextureFromSurface(r, textMessage);
		SDL_FRect textRect = { center[0] - textMessage->w / 2, center[1] - textMessage->h / 2, textMessage->w, textMessage->h };

		SDL_RenderTexture(r, tex, NULL, &textRect);

		SDL_DestroyTexture(tex);
		SDL_DestroySurface(textMessage);
	}
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
		offsets.push_back(MVector(2, 0.0, -100.0 * (i - (terminals - 1) / 2.0)));
	}
	init(_pos);
}

void Device::init(MVector _pos)
{
	position = _pos;
	for (int connectionIndex = 0; connectionIndex < offsets.size(); connectionIndex++)
	{
		Node* newNode = new Node();
		Terminal* newTerm = new Terminal(this, newNode, connectionIndex, offsets[connectionIndex]);
		terminals.push_back(newTerm);
	}
}

Node* mergeNodes(Node* A, Node* B)
{
	Node* C = new Node();

	for (int i = 0; i < A->junctions.size(); i++)
	{
		C->junctions.push_back(A->junctions[i]);
	}
	for (int i = 0; i < B->junctions.size(); i++)
	{
		C->junctions.push_back(B->junctions[i]);
	}

	delete A;
	delete B;

	for (int i = 0; i < C->junctions.size(); i++)
	{
		C->junctions[i]->node = C;
	}

	return C;
}

Node* connectJunction(Junction* A, Junction* B)
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

		MVector pos = terminal->getGlobalPosition();

		if (termIndex < terminals.size() - 1)
		{
			Terminal* nextTerminal = terminals[termIndex + 1];
			MVector nextPos = nextTerminal->getGlobalPosition();
			SDL_SetRenderDrawColor(r, 255, 0, 0, 255);
			SDL_RenderLine(r, pos[0], pos[1], nextPos[0], nextPos[1]);
		}

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
		int digits = 1 + log10(terminal->node->voltage);
		if (digits < 1) { digits = 1; }
		int decimals = 2;
		int maxlen = digits + 1 + decimals;
		if (terminal->node->voltage < 0)
		{
			maxlen += 1;
		}
		if (maxlen > s_voltage.size())
		{
			maxlen = s_voltage.size();
		}
		s_voltage = s_voltage.substr(0, maxlen);
		
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

std::vector<Node*> getNodeList()
{
	return nodeList;
}

std::vector<Junction*> getJunctionList()
{
	return junctionList;
}

Node* getNodeByID(int id)
{
	for (int i = 0; i < nodeList.size(); i++)
	{
		if (nodeList[i]->id == id)
		{
			return nodeList[i];
		}
	}
	return nullptr;
}

Terminal* Terminal::getOtherTerminal()
{
	if (device->terminals.size() < 2)
	{
		return device->terminals[0];
	}
	if (device->terminals[0] == this)
	{
		return device->terminals[1];
	}
	else
	{
		return device->terminals[0];
	}
}

MVector Terminal::getGlobalPosition()
{
	double len = pos.magnitude();
	double angle = device->rotation * 3.14159265358979323846 / 180.0;

	double angle_0 = atan2(pos[1], pos[0]);
	angle += angle_0;

	MVector globalOffset(2, len*cos(angle), len * sin(angle));
	
	MVector globalPos = device->position + globalOffset;

	return globalPos;
}

void addEmptyEquation(Matrix* solver, std::vector<double*>& equations)
{
	double* equation = new double[solver->cols];
	for (int c = 0; c < solver->cols; c++)
	{
		equation[c] = 0;
	}
	equations.push_back(equation);
}

void resetEquation(Matrix* solver, double* equation)
{
	for (int c = 0; c < solver->cols; c++)
	{
		equation[c] = 0;
	}
}

void Node::generateEquations(Matrix* solver, vector<double*>& equations, vector<int>& addedNodes, bool& forced)
{
	if (std::find(addedNodes.begin(), addedNodes.end(), id) != addedNodes.end()) { return; }

	addedNodes.push_back(id);
	addEmptyEquation(solver, equations);

	for (int otherIndex = 0; otherIndex < junctions.size(); otherIndex++)
	{
		if (junctions[otherIndex]->type == 0) { continue; }
		Terminal* terminal = (Terminal*)junctions[otherIndex];
		Device* device = terminal->device;
		Node* otherNode = terminal->getOtherTerminal()->node;

		if (device->deviceType == 0) //Ground
		{
			
			resetEquation(solver, equations[0]);
			forced = true;
			equations[0][id] = 1;
		}

		if (device->deviceType == 1 && !forced) //Resistor
		{
			double coef = 1.0 / (device->value);

			equations[0][id] += coef;
			equations[0][otherNode->id] += -coef;
		}

		if (device->deviceType == 2) //Voltage Source (Make SuperNode)
		{
			addEmptyEquation(solver, equations);
			int index = equations.size() - 1;

			equations[index][id] = 1;
			equations[index][otherNode->id] = -1;

			if (terminal == device->terminals[0]) //T0 - T1 = - V or //T1 - T0 = V
			{
				equations[index][solver->cols - 1] = -device->value;
			}
			else
			{
				equations[index][solver->cols - 1] = device->value;
			}
			
			otherNode->generateEquations(solver, equations, addedNodes, forced);
		}
	}
	return;
}