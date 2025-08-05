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

		DevicePreset dev = readPresetFile(entry.path());
		presets.push_back(dev);
		string cat = dev.category;
		
		if (library.count(cat) == 0) { library[cat] = {}; categories.push_back(cat); }
		library[cat].push_back(dev);

	}
}

DevicePreset DeviceLibrary::readPresetFile(std::filesystem::path _path)
{
	string _name;
	string _cat;
	int _type;
	int _terminals;
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
			_cat = string(line);
		}
		else if (lineidx == 2)
		{
			string typestring(line);
			_type = stoi(typestring);
		}
		else if (lineidx == 3)
		{
			string termstring(line);
			_terminals = stoi(termstring);
		}
		else if (lineidx == 4)
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


	return { _name, _cat, _type, _terminals, props, _imgpath};
}