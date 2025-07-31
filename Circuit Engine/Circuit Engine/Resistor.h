#pragma once
#include "Node.h"

class Resistor : Device
{
public:
	double width = 20;
	double resistance = 10;
	void setOffsets() override;
	double calculateNodeVoltages(int terminalIndex, double step) override;
	double calculateCurrentOutOfNode(int terminalIndex, double step) override;
};