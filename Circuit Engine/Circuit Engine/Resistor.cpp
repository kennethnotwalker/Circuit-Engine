#include "Resistor.h"
#include "MathVector.h"

void Resistor::setOffsets()
{
	offsets = {
		MVector(2, -width/2.0, 0),
		MVector(2, width/2.0, 0)
	};
}

double Resistor::calculateNodeVoltages(int terminalIndex, double step) //T0 is +, T1 is -
{
	//sum of current in connected nodes = 0
	if (terminalIndex == 0)
	{
		return terminals[1]->node->voltage + resistance * terminals[1]->current;
	}

	if (terminalIndex == 1)
	{
		return terminals[0]->node->voltage + resistance * terminals[0]->current;
	}
}

double Resistor::calculateCurrentOutOfNode(int terminalIndex, double step)
{
	double current = (terminals[0]->node->voltage - terminals[1]->node->voltage) / resistance;

	if (terminalIndex == 0)
	{
		return -current;
	}

	if (terminalIndex == 1)
	{
		return current;
	}
}