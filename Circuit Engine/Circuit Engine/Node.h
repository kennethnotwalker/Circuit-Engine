#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "MathVector.h"
#include <vector>

class Node;
class Terminal;
class Device;

class Node
{
	public:
		double x, y;
		int id = 0;
		double voltage = 0;
		bool calculated = false;
		bool forced = false;
		std::vector<Terminal*> connected_terminals;
		Node();
		~Node();

		MVector getPosition()
		{
			return MVector(2, x, y);
		}
};


class Device
{
public:
	Terminal* getTerminal(int index);
	int id = -1;
	int deviceType = 0; //0 - Ground, 1 - Resistor, 2 - Voltage Source, 3 - Current Source
	double value;
	MVector position = NULL;
	std::vector<Terminal*> terminals;
	std::vector<MVector> offsets;

	Device(MVector _pos, int type, int terminals);

	void init(MVector _pos);

	void setOffsets();
	void stepUpdate(double step);

	void render(SDL_Renderer* r);

	
};

class Terminal
{
public:
	Device* device;
	Node* node;
	int terminalIndex;
	MVector pos = MVector(2, 0, 0);

	Terminal(Device* _device, Node* _node, int _termIndex, MVector _pos)
	{
		device = _device;
		node = _node;
		node->connected_terminals.push_back(this);
		terminalIndex = _termIndex;
		pos = _pos;
	}

};

Node* mergeNodes(Node* A, Node* B);
Node* connectTerminals(Terminal* A, Terminal* B);