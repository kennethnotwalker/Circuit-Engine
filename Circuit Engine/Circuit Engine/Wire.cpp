#include "Node.h"
#include "vector"
#include "Constants.h"

Wire::Wire(MVector _from, MVector _to)
{
	from = _from;
	to = _to;

	fromJunc = nullptr;
	toJunc = nullptr;

	std::vector<Node*> nodes = getNodeList();
	std::vector<Junction*> junctions = getJunctionList();
	
	for (int nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++)
	{
		Node* node = nodes[nodeIndex];
		for (int terminalIndex = 0; terminalIndex < node->junctions.size(); terminalIndex++)
		{
			Terminal* terminal = (Terminal*)node->junctions[terminalIndex];
			if (MVector::distance(terminal->getGlobalPosition(), from) <= 1 && fromJunc == nullptr)
			{
				fromJunc = (Junction*)terminal;
			}
			if (MVector::distance(terminal->getGlobalPosition(), to) <= 1 && toJunc == nullptr)
			{
				toJunc = (Junction*)terminal;
			}
		}
	}

	for (int juncIndex = 0; juncIndex < junctions.size(); juncIndex++)
	{
		Junction* junction = junctions[juncIndex];
		if (junction->type == 1) { continue;; }
		if (MVector::distance(junction->getGlobalPosition(), from) < 10 && fromJunc == nullptr)
		{
			fromJunc = junction;
		}
		if (MVector::distance(junction->getGlobalPosition(), to) < 10 && toJunc == nullptr)
		{
			toJunc = junction;
		}
	}

	if (fromJunc == nullptr)
	{
		fromJunc = new Junction(from);
		fromJunc->node = new Node();
		fromJunc->node->junctions.push_back(fromJunc);
	}

	if (toJunc == nullptr)
	{
		toJunc = new Junction(to);
		toJunc->node = new Node();
		toJunc->node->junctions.push_back(toJunc);
	}

	if (fromJunc != nullptr && toJunc != nullptr && fromJunc != toJunc)
	{
		connectJunction(fromJunc, toJunc);
		connected = true;
	}

	
}

void Wire::render(SDL_Renderer* r)
{
	SDL_SetRenderDrawColor(r, 0, 0, 255, 255);

	SDL_RenderLine(r, from[0], from[1], to[0], to[1]);
}

MVector Wire::snap(MVector _from, MVector _to)
{
	MVector snapped = _to;

	double xDiff = abs(_to[0] - _from[0]);
	double yDiff = abs(_to[1] - _from[1]);

	if (xDiff < yDiff)
	{
		snapped = MVector(2, _from[0], _to[1]);
	}
	else
	{
		snapped = MVector(2, _to[0], _from[1]);
	}
	return snapped;
}