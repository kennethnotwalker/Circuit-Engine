#pragma once
#include "Node.h"

class VoltageSource : Device
{
public:
	double height = 10;
	double voltage = 10;
	VoltageSource(MVector _pos) : Device(_pos) {};
	void setOffsets() override;
	double calculateNodeVoltages(int terminalIndex, double step) override;
	double calculateCurrentOutOfNode(int terminalIndex, double step) override;
};

class Ground : Device
{
public:
	double height = 10;
	double voltage = 10;
	Ground(MVector _pos) : Device(_pos) {};
	void setOffsets() override;
	double calculateNodeVoltages(int terminalIndex, double step) override;
	double calculateCurrentOutOfNode(int terminalIndex, double step) override;
};