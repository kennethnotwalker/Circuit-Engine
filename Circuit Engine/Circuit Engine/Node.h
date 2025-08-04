#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include "MathVector.h"
#include <vector>
#include <string>
#include "ImageLoader.h"
#include "ComplexMatrix.h"

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
		complex voltage = 0;
		bool calculated = false;
		bool forced = false;
		std::vector<Junction*> junctions;
		Node();
		~Node();

		MVector getPosition()
		{
			return MVector(2, x, y);
		}

		void generateEquations(ComplexMatrix* solver, vector<complex*>& equations, vector<int>& addedNodes, bool& forced);

		void render(SDL_Renderer* r);
};


class Device
{
public:
	vector<std::string> propertyList;
	map<std::string, complex> properties;
	map<std::string, bool> hasProperty;
	Terminal* getTerminal(int index);
	int id = -1;
	int deviceType = 0; //0 - Ground, 1 - Resistor, 2 - Voltage Source, 3 - Current Source
	double rotation = 0;
	complex value;
	MVector position = NULL;
	SDL_Texture* texture;
	std::vector<Terminal*> terminals;
	std::vector<MVector> offsets;

	Device(MVector _pos, int type, int terminals, std::string textureAlias, ImageLoader& loader);

	void init(MVector _pos);

	void setOffsets();
	void stepUpdate(double step);

	void render(SDL_Renderer* r);

	void setProperty(std::string name, complex val);

	complex getProperty(std::string name);
	complex* getPropertyReference(std::string name);

	SDL_FRect getSelectionRect();

	
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

void addEmptyEquation(ComplexMatrix* solver, std::vector<complex*>& equations);
void resetEquation(ComplexMatrix* solver, complex* equation);

MVector nodeSnap(MVector in);