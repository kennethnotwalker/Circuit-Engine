#include "DevicePreset.h"
#include <string>
#include <vector>
#include "Node.h"
#include <filesystem> 
#include <iostream>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

string presetDirectory("presets/");

DeviceLibrary::DeviceLibrary()
{
	for (const auto& entry : fs::directory_iterator(presetDirectory))
	{
		cout << "loading " << entry.path() << endl;
		presets.push_back(readPresetFile(entry.path()));
	}
}

DevicePreset DeviceLibrary::readPresetFile(std::filesystem::path _path)
{
	string _name;
	int _type;
	string _imgpath;
	vector<string> props;


	ifstream presetIn;

	char data[1024];
	u8string u8path = _path.u8string();

	presetIn.open(_path);

	std::string s_data(data);
	int lineidx = 0;
	while (!presetIn.eof())
	{
		char line[1024];
		presetIn.getline(line, sizeof(line));
		if (lineidx == 0)
		{
			_name = string(line);
		}
		else if (lineidx == 1)
		{
			string typestring(line);
			_type = stoi(typestring);
		}
		else if (lineidx == 2)
		{
			_imgpath = string(line);
		}
		else
		{
			string propstring(line);
			props.push_back(propstring);
		}

		lineidx++;
	}
	presetIn.close();


	return { _name, _type, props, _imgpath};
}