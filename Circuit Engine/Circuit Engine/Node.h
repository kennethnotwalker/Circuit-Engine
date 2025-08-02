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
class Junction;
class Wire;

class Node
{
	public:
		double x, y;
		int id = 0;
		double voltage = 0;
		bool calculated = false;
		bool forced = false;
		std::vector<Junction*> junctions;
		Node();
		~Node();

		MVector getPosition()
		{
			return MVector(2, x, y);
		}

		void generateEquations(Matrix* solver, vector<double*>& equations, vector<int>& addedNodes, bool& forced);

		void render(SDL_Renderer* r);
};


class Device
{
public:
	Terminal* getTerminal(int index);
	int id = -1;
	int deviceType = 0; //0 - Ground, 1 - Resistor, 2 - Voltage Source, 3 - Current Source
	double rotation = 0;
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

class Wire
{
public:
	MVector from = MVector(2, 0, 0);
	MVector to = MVector(2, 0, 0);

	Junction* fromJunc;
	Junction* toJunc;

	bool connected;

	Wire(MVector _from, MVector _to);

	void render(SDL_Renderer* r);

	static MVector snap(MVector _from, MVector _to);
};

class Junction
{
public:
	std::vector<Wire*> wires;
	Node* node;

	MVector pos = MVector(2, 0, 0);

	int type = 0;

	Junction(MVector _pos) { pos = _pos; };

	virtual MVector getGlobalPosition() { return pos; }
};

class Terminal : public Junction
{
public:
	Device* device;
	
	int terminalIndex;
	

	Terminal(Device* _device, Node* _node, int _termIndex, MVector _pos) : Junction(_pos)
	{
		type = 1;
		device = _device;
		node = _node;
		node->junctions.push_back(this);
		terminalIndex = _termIndex;
	}

	Terminal* getOtherTerminal();
	MVector getGlobalPosition() override;

};

Node* mergeNodes(Node* A, Node* B);
Node* connectJunction(Junction* A, Junction* B);

std::vector<Node*> getNodeList();
std::vector<Junction*> getJunctionList();

Node* getNodeByID(int id);

void addEmptyEquation(Matrix* solver, std::vector<double*>& equations);
void resetEquation(Matrix* solver, double* equation);