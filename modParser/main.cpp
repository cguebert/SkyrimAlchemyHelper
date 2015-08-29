#include <iostream>
#include <fstream>

#include "Ingredients.h"
#include "Mod.h"

using namespace std;

float round(float v, int d)
{
	float p = pow(10.0f, d);
	return std::round(v * p) / p;
}

void exportConfig(const Config& config)
{
	ofstream modsFile("data/Plugins.txt");
	for (const auto& mod : config.modsList)
		modsFile << mod << endl;

	ofstream effectsFile("data/Effects.txt");
	for (const auto& effect : config.magicalEffectsList)
	{
		effectsFile << effect.second << endl;
		effectsFile << hex << uppercase << effect.first << dec << endl;
	}

	ofstream ingredientsFile("data/Ingredients.txt");
	for (const auto& ingr : config.ingredientsList.ingredients())
	{
		ingredientsFile << ingr.name << endl << ingr.modName << endl;
		ingredientsFile << hex << uppercase << ingr.id << dec << endl;
		for (const auto& effect : ingr.effects)
		{
			ingredientsFile << hex << uppercase << effect.id << dec << " ";
			ingredientsFile << round(effect.magnitude, 6) << " ";
			ingredientsFile << effect.duration << endl;
		}
	}
}

void absolutePathTest(Config& config)
{
	Mod::parse("F:/Jeux/Skyrim/Data/Skyrim.esm", config);
	Mod::parse("F:/Jeux/SkyrimModOrganizer/mods/83Willows 101BugsHD/83Willows_101BUGS_V4_LowRes.esp", config);
	Mod::parse("F:/Jeux/SkyrimModOrganizer/mods/Patchus Maximus/PatchusMaximus.esp", config);
}

void modOrganizerTest(Config& config)
{
	const std::string pluginsFileName = "F:/Jeux/SkyrimModOrganizer/profiles/Civil War/Plugins.txt";
//	const std::string pluginsFileName = "C:/Users/Christophe/AppData/Local/Skyrim/Plugins.txt";
	fstream pluginsList(pluginsFileName);
	array<char, 256> pluginName{};
	string dir = "F:/Jeux/Skyrim/Data/";
	while (pluginsList.getline(&pluginName[0], 256))
	{
		if (pluginName[0] == '#')
			continue;
		cout << pluginName.data() << endl;

		Mod::parse(dir + pluginName.data(), config);
	}
}

int main(int argc, char** argv)
{
	Config config;

//	absolutePathTest(config);
//	modOrganizerTest(config);	
	
	exportConfig(config);

//	Save::parse("data/save.ess");

	system("Pause");
	return 0;
}

