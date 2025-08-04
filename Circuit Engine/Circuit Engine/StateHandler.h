#pragma once
#include <string>
#include <vector>
#include "Node.h"
#include "DevicePreset.h"

using namespace std;

class StateHandler
{
public:
	enum InputMode
	{
		MODE_PLACE,
		MODE_SELECT,
		MODE_MOVE
	};

	InputMode mode;
	DevicePreset preset;
	Device* selectedDevice;
	bool dragging;

	StateHandler() {
		mode = MODE_SELECT;
		selectedDevice = nullptr;
		dragging = false;
	};
};