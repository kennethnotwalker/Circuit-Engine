#include "MathVector.h"
#include "ComplexMatrix.h"
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
#include "ImageLoader.h"


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

void Device::setProperty(std::string name, complex val)
{
	properties[name] = val;
}

complex Device::getProperty(std::string name)
{
	return properties[name];
}

void Node::render(SDL_Renderer* r)
{
	MVector center = junctions[0]->getGlobalPosition();
	SDL_SetRenderDrawColor(r, (255.0/2)*(id%2), (255.0 / 3) * (id % 3), (255.0 / 4) * (id % 4), 255);
	for (int termIndex = 0; termIndex < junctions.size(); termIndex++) {

		Terminal* other = (Terminal*)junctions[termIndex];

		MVector pos = other->getGlobalPosition();

		double xDist = abs(center[0] - pos[0]);
		double yDist = abs(center[1] - pos[1]);

		if (xDist > yDist)
		{
			SDL_RenderLine(r, pos[0], pos[1], pos[0], center[1]);
			SDL_RenderLine(r, pos[0], center[1], center[0], center[1]);
		}
		else
		{
			SDL_RenderLine(r, pos[0], pos[1], center[0], pos[1]);
			SDL_RenderLine(r, center[0], pos[1], center[0], center[1]);
		}

			
	}
	TTF_Font* calibri = TTF_OpenFont("calibri-regular.ttf", 18);

	SDL_Color green = { 0, 255, 0 };

	std::string s_voltage = std::to_string(voltage.real);
	int digits = 1 + log10(voltage.real);
	if (digits < 1) { digits = 1; }
	int decimals = 2;
	int maxlen = digits + 1 + decimals;
	if (voltage.real < 0)
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
	SDL_FRect textRect = { center[0] - textMessage->w / 2, center[1] - textMessage->h / 2, textMessage->w, textMessage->h };

	SDL_RenderTexture(r, tex, NULL, &textRect);

	SDL_DestroyTexture(tex);
	SDL_DestroySurface(textMessage);
	
}

void Device::setOffsets()
{
	return;
}

void Device::stepUpdate(double step)
{
	return;
}

Device::Device(MVector _pos, int type, int terminals, std::string textureAlias, ImageLoader& loader)
{
	deviceType = type;
	for (int i = 0; i < terminals; i++)
	{
		if (i == 0)
		{
			offsets.push_back(MVector(2, 0.0, 50.0));
		}
		if(i == 1)
		{
			offsets.push_back(MVector(2, 0.0, -50.0));
		}
		
	}
	texture = loader.getImage(textureAlias);
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
	double h = 100;
	double w = h*(texture->w)/(texture->h);
	SDL_FRect texRect = { (float)(position[0] - w/2),(float)(position[1] - h/2), w, h};

	SDL_RenderTextureRotated(r, texture, NULL, &texRect, rotation, NULL, SDL_FLIP_NONE);

	TTF_Font* calibri = TTF_OpenFont("calibri-regular.ttf", 18);

	SDL_Color green = { 255, 0, 0 };

	std::string s_voltage = std::to_string(value.real);
	int digits = 1 + log10(value.real);
	if (digits < 1) { digits = 1; }
	int decimals = 2;
	int maxlen = digits + 1 + decimals;
	if (value.real < 0)
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
	SDL_FRect textRect = { position[0] - textMessage->w / 2, position[1] - textMessage->h / 2, textMessage->w, textMessage->h };

	SDL_RenderTexture(r, tex, NULL, &textRect);

	SDL_DestroyTexture(tex);
	SDL_DestroySurface(textMessage);
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

void addEmptyEquation(ComplexMatrix* solver, std::vector<complex*>& equations)
{
	complex* equation = new complex[solver->cols];
	for (int c = 0; c < solver->cols; c++)
	{
		equation[c] = 0;
	}
	equations.push_back(equation);
}

void resetEquation(ComplexMatrix* solver, complex* equation)
{
	for (int c = 0; c < solver->cols; c++)
	{
		equation[c] = 0;
	}
}

void Node::generateEquations(ComplexMatrix* solver, vector<complex*>& equations, vector<int>& addedNodes, bool& forced)
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
			complex coef = 1.0 / (device->getProperty("resistance"));

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
				equations[index][solver->cols - 1] = -device->getProperty("voltage");
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