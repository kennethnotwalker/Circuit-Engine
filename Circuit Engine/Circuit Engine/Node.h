#pragma once

#include "MathVector.h"

class Node
{
	int x, y;
	public:
		Node(MVector position)
		{
			x = position[0];
			y = position[1];
		}
};