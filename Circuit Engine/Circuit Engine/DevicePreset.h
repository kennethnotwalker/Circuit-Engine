#pragma once
#include <string>
#include <vector>
#include "Node.h"
#include <filesystem> 

using namespace std;

struct DevicePreset
{
	string name;
	int baseType;
	vector<string> properties;
	string imgPath;
};

class DeviceLibrary
{
public:
	vector<DevicePreset> presets;
	DeviceLibrary();
	DevicePreset readPresetFile(std::filesystem::path _path);
};
