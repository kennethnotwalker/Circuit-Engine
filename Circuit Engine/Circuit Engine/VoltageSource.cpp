#include "VoltageSource.h"
#include "MathVector.h"

void Ground::setOffsets()
{
	offsets = {
		MVector(2, height / 2.0, 0),
	};
}

double Ground::calculateNodeVoltages(int terminalIndex, double step)
{
	return 0;
}

double Ground::calculateCurrentOutOfNode(int terminalIndex, double step)
{
	return terminals[terminalIndex]->exclusiceKCL(step);
}

void VoltageSource::setOffsets()
{
	offsets = {
		MVector(2, -height / 2.0, 0),
		MVector(2, height / 2.0, 0)
	};
}

double VoltageSource::calculateNodeVoltages(int terminalIndex, double step) //T0 is -, T1 is +
{
	//sum of current in connected nodes = 0
	if (terminalIndex == 0)
	{
		return terminals[0]->node->voltage;
	}

	if (terminalIndex == 1)
	{
		return terminals[0]->node->voltage + voltage;
	}
}

double VoltageSource::calculateCurrentOutOfNode(int terminalIndex, double step)
{
	return terminals[terminalIndex]->exclusiceKCL(step);
}