#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>
#include "MathVector.h"
#include "RenderObject.h"
#include <vector>

class Node;
class Terminal;
class Device;

class Node : RenderObject
{
	public:
		double x, y;
		double voltage = 0;
		bool calculated = false;
		std::vector<Terminal*> connected_terminals;
		Node(MVector position)
		{
			x = position[0];
			y = position[1];
		}
		void render (SDL_Renderer* r) override;

		MVector getPosition()
		{
			return MVector(2, x, y);
		}
};


class Device : RenderObject
{
public:
	MVector position = NULL;
	std::vector<Terminal*> terminals;
	std::vector<MVector> offsets;

	Device(MVector _pos);

	virtual void setOffsets() = 0;
	virtual double calculateNodeVoltages(int terminalIndex, double step) = 0;
	virtual double calculateCurrentOutOfNode(int terminalIndex, double step) = 0;

	virtual void render(SDL_Renderer* r);
};

class Terminal
{
public:
	Device* device;
	Node* node;
	int terminalIndex;
	MVector pos = MVector(2, 0, 0);
	double current = 0; //current going out

	Terminal(Device* _device, Node* _node, int _termIndex, MVector _pos)
	{
		device = _device;
		node = _node;
		node->connected_terminals.push_back(this);
		terminalIndex = _termIndex;
		pos = _pos;
	}

	double exclusiceKCL(double step)
	{
		double incomingCurrent = 0;
		for (int i = 0; i < node->connected_terminals.size(); i++)
		{
			Terminal* other = node->connected_terminals[i];
			if (other != this)
			{
				incomingCurrent += other->getCurrentGoingOut(step);
			}
		}
		return -incomingCurrent;
	}

	double getCurrentGoingOut(double step)
	{
		return device->calculateCurrentOutOfNode(terminalIndex, step);
	}

	double getVoltage(double step)
	{
		return device->calculateNodeVoltages(terminalIndex, step);
	}
};