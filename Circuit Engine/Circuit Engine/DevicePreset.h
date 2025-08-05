#pragma once
#include <string>
#include <vector>
#include <map>
#include "Node.h"
#include <filesystem> 

using namespace std;

class DeviceLibrary
{
public:
	vector<string> categories;
	map<string, vector<DevicePreset>> library;
	vector<DevicePreset> presets;
	DeviceLibrary();
	DevicePreset readPresetFile(std::filesystem::path _path);
};
