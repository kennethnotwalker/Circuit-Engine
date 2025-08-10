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
#include "Constants.h"


int GLOBAL_ID_COUNTER = 0;
double timeElapsed = 0;
std::vector<Node*> nodeList;
std::vector<Junction*> junctionList;

MVector SNAP_SIZE(2, 50.0, 50.0);

MVector nodeSnap(MVector in)
{
	double x = in[0] / SNAP_SIZE[0];
	double y = in[1] / SNAP_SIZE[1];
	x = round(x) * SNAP_SIZE[0];
	y = round(y) * SNAP_SIZE[1];

	return MVector(2, x, y);
}

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

SDL_FRect Device::getSelectionRect()
{
	double _w = 100;
	double _h = _w * texture->h / texture->w;
	double _a = rotation * 3.1415926535 / 180.0;
	//rotate
	double w = abs(_w * cos(_a)) + abs(_h * sin(_a));
	double h = abs(_w * sin(_a)) + abs(_h * cos(_a));

	SDL_FRect selectionRect = { (float)(position[0] - w / 2),(float)(position[1] - h / 2), w, h };
	return selectionRect;
}

void Device::setProperty(std::string name, complex val)
{
	if (hasProperty[name] == false)
	{
		propertyList.push_back(name);
		history[name] = {};
	}
	properties[name] = val;
	hasProperty[name] = true;
}

complex Device::getProperty(std::string name)
{
	return properties[name];
}

complex* Device::getPropertyReference(std::string name)
{
	return &properties[name];
}

void Node::render(SDL_Renderer* r)
{
	MVector center = MVector(2, 0, 0);
	for (int termIndex = 0; termIndex < junctions.size(); termIndex++)
	{
		Terminal* other = (Terminal*)junctions[termIndex];

		MVector pos = other->getGlobalPosition();
		center = center + pos;
	}

	center = center * (1.0 / junctions.size());

	SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
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
	displayNumber(voltage.real, center, r);
}

void displayNumber(double num, MVector center, SDL_Renderer* r)
{
	std::string s_num = std::to_string(num);
	int digits = 1 + log10(num);
	if (digits < 1) { digits = 1; }
	int decimals = 2;
	int maxlen = digits + 1 + decimals;
	if (num < 0)
	{
		maxlen += 1;
	}
	if (maxlen > s_num.size())
	{
		maxlen = s_num.size();
	}
	s_num = s_num.substr(0, maxlen);
	displayText(s_num, center, r);
}

void displayText(std::string text, MVector center, SDL_Renderer* r)
{
	TTF_Font* calibri = TTF_OpenFont("calibri-regular.ttf", 18);

	SDL_Color green = { 0, 255, 0 };

	SDL_Surface* textMessage = TTF_RenderText_Solid(calibri, text.c_str(), text.size(), green);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(r, textMessage);
	SDL_FRect textRect = { center[0] - textMessage->w / 2, center[1] - textMessage->h / 2, textMessage->w, textMessage->h };

	SDL_RenderTexture(r, tex, NULL, &textRect);

	SDL_DestroyTexture(tex);
	SDL_DestroySurface(textMessage);
	TTF_CloseFont(calibri);
}

void Device::setOffsets()
{
	return;
}

void Device::stepUpdate(double step)
{
	if (hasProperty["capacitance"] && getProperty("capacitance").real > 0)
	{
		complex dV = -terminals[0]->current / getProperty("capacitance");
		complex V = getProperty("voltage") + dV*step;

		setProperty("voltage", V);
	}

	if (hasProperty["inductance"] && getProperty("inductance").real > 0 && history["current"].size() > 3)
	{
		complex dI = (getProperty("current") - history["current"][history["current"].size() - 2]) / step;
		complex V = -getProperty("inductance")*dI;

		complex I = getProperty("current");
		if (abs(I) > 0) {
			complex R = V / I;

			setProperty("resistance", R);
		}
	}

	if (hasProperty["off_time"] && getProperty("off_time").real > 0 && timeElapsed >= getProperty("off_time").real)
	{
		complex Von = getProperty("on_voltage");
		complex Voff = getProperty("off_voltage");

		complex V = Voff;
		if (timeElapsed < getProperty("off_time").real + getProperty("change_time").real)
		{
			V = Voff + (Von - Voff) * ((getProperty("off_time").real + getProperty("change_time").real) - timeElapsed) / getProperty("change_time").real;
		}

		setProperty("voltage", V);
	}
	else if (hasProperty["on_voltage"])
	{
		setProperty("voltage", getProperty("on_voltage"));
	}
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
	setProperty("current", 0);
	init(_pos);
}

void Device::init(MVector _pos)
{
	position = nodeSnap(_pos);
	for (int connectionIndex = 0; connectionIndex < offsets.size(); connectionIndex++)
	{
		Node* newNode = new Node();
		Terminal* newTerm = new Terminal(this, newNode, connectionIndex, offsets[connectionIndex]);
		terminals.push_back(newTerm);
	}
}

void Device::storeProperties()
{
	for (int i = 0; i < propertyList.size(); i++)
	{
		history[propertyList[i]].push_back(getProperty(propertyList[i]));
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

	for (int i = 0; i < terminals.size(); i++)
	{
		SDL_SetRenderDrawColor(r, 255, 255, 255, 255);
		double _w = 20;
		double _h = 20;
		SDL_FRect termRect = { terminals[i]->getGlobalPosition()[0] - _w / 2, terminals[i]->getGlobalPosition()[1] - _h / 2, _w, _h };
		SDL_RenderRect(r, &termRect);
		//displayNumber(terminals[i]->id, terminals[i]->getGlobalPosition(), r);
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

Terminal* getTerminalByID(int id)
{
	for (int i = 0; i < nodeList.size(); i++)
	{
		Node* node = nodeList[i];
		for (int t = 0; t < node->junctions.size(); t++)
		{
			Terminal* terminal = (Terminal*)node->junctions[t];
			if (terminal->id == id) {
				return terminal;
			}
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

void Device::resetCurrentCalculations()
{
	for (int t = 0; t < terminals.size(); t++)
	{
		terminals[t]->foundCurrent = false;
	}
}

void Device::calculateCurrent()
{
	for (int t = 0; t < terminals.size(); t++)
	{
		terminals[t]->getCurrent();
	}
}

complex Terminal::getCurrent()
{
	if (foundCurrent) { return current; }
	complex val = 0;
	foundCurrent = true;
	
	if (device->hasProperty["resistance"])
	{
		val = (node->voltage - getOtherTerminal()->node->voltage) / device->getProperty("resistance");
	}
	else
	{
		for (int i = 0; i < node->junctions.size(); i++)
		{
			Terminal* other = (Terminal*)node->junctions[i];
			val -= other->getCurrent();
		}
	}


	current = val;
	return val;
}

bool Node::isGrounded()
{
	for (int t = 0; t < junctions.size(); t++)
	{
		Terminal* terminal = (Terminal*)junctions[t];
		if (terminal->device->deviceType == 0) //terminal is grounded
		{
			return true;
		}
	}
	return false;
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

		if (device->deviceType == 1 && !forced && abs(device->getProperty("resistance")) > MINIMUM_RESISTANCE) //Resistor
		{
			complex coef = 1.0 / (device->getProperty("resistance"));
			equations[0][id] += coef;
			equations[0][otherNode->id] += -coef;
		}
		else if (device->deviceType == 1 && !forced) //Equate nodes (0-volt power supply)
		{
			addEmptyEquation(solver, equations);
			int index = equations.size() - 1;

			equations[index][id] = 1;
			equations[index][otherNode->id] = -1;

			otherNode->generateEquations(solver, equations, addedNodes, forced);
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
				equations[index][solver->cols - 1] = device->getProperty("voltage");
			}
			
			otherNode->generateEquations(solver, equations, addedNodes, forced);
		}
	}
	return;
}

void Node::generateCurrentEquations(ComplexMatrix* solver, vector<complex*>& equations, vector<int>& addedNodes, bool& forced)
{
	if (std::find(addedNodes.begin(), addedNodes.end(), id) != addedNodes.end()) { return; }

	addedNodes.push_back(id);
	addEmptyEquation(solver, equations);

	for (int otherIndex = 0; otherIndex < junctions.size(); otherIndex++)
	{
		Terminal* terminal = (Terminal*)junctions[otherIndex];
		if (terminal->foundCurrent) { continue; }
		terminal->foundCurrent = true;
		Device* device = terminal->device;
		if (device->hasProperty["resistance"] && abs(device->getProperty("resistance")) > MINIMUM_RESISTANCE)
		{
			complex current = (voltage - terminal->getOtherTerminal()->node->voltage) / device->getProperty("resistance");
			addEmptyEquation(solver, equations);
			int index = equations.size() - 1;
			equations[index][terminal->id] = 1;
			equations[index][solver->cols - 1] = current;
		}
		else if (device->terminals.size() == 2) //assume 0 resistance
		{
			addEmptyEquation(solver, equations);
			int index = equations.size() - 1;
			equations[index][terminal->id] = 1;
			equations[index][terminal->getOtherTerminal()->id] = 1;
		}
		if (terminal->device->deviceType == 0)
		{
			vector<Node*> nodes;
			for (int n = 0; n < nodes.size(); n++)
			{
				if (nodes[n]->isGrounded())
				{
					nodes[n]->generateCurrentEquations(solver, equations, addedNodes, forced);
				}
			}
		}
		else
		{
			equations[0][terminal->id] = 1;
		}
		

		
	}
	return;
}

void elapseTime(double t)
{
	timeElapsed += t;
}

double getElapsedTime()
{
	return timeElapsed;
}