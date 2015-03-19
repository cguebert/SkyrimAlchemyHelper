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
	ofstream modsFile("Plugins.txt");
	for (const auto& mod : config.modsList)
		modsFile << mod << endl;

	ofstream effectsFile("Effects.txt");
	for (const auto& effect : config.magicalEffectsList)
	{
		effectsFile << effect.second << endl;
		effectsFile << hex << uppercase << effect.first << dec << endl;
	}

	ofstream ingredientsFile("Ingredients.txt");
	for (const auto& ingr : config.ingredientsList.ingredients())
	{
		ingredientsFile << ingr.name << endl << ingr.modName << endl;
		for (const auto& effect : ingr.effects)
		{
			ingredientsFile << hex << uppercase << effect.id << dec << " ";
			ingredientsFile << round(effect.magnitude, 6) << " ";
			ingredientsFile << effect.duration << endl;
		}
	}
}

int main(int argc, char** argv)
{
	Config config;

	const std::string pluginsFileName = "F:/Jeux/SkyrimModOrganizer/profiles/Civil War/Plugins.txt";
//	const std::string pluginsFileName = "C:/Users/Christophe/AppData/Local/Skyrim/Plugins.txt";
	fstream pluginsList(pluginsFileName);
	array<char, 256> pluginName{};
	string dir = "F:/Jeux/Skyrim/Data/";
	Mod::parse(dir + "Skyrim.esm", config);
	while (pluginsList.getline(&pluginName[0], 256))
	{
		cout << pluginName.data() << endl;

		Mod::parse(dir + pluginName.data(), config);
	}

/*	if(argc < 2)
		Mod::parse("data/test.esp", config);
	else
		Mod::parse(argv[1], config);
		*/
//	Mod::parse("F:/Jeux/Skyrim/Data/Skyrim.esm", config);
//	Mod::parse("F:/Jeux/SkyrimModOrganizer/mods/Patchus Maximus/PatchusMaximus.esp", config);

	exportConfig(config);
	
	//"C:/Users/Christophe/AppData/Local/Skyrim/plugins.txt";
	
	return 0;
}

