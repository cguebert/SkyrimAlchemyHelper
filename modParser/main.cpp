#include <iostream>
#include <fstream>

#include "Config.h"
#include "Mod.h"

using namespace std;

float round(float v, int d)
{
	float p = pow(10.0f, d);
	return round(v * p) / p;
}

void exportConfig(const modParser::Config& config)
{
	ofstream modsFile("data/Plugins.txt");
	for (const auto& mod : config.modsList)
		modsFile << mod << endl;
	modsFile.close();

	ifstream modsIn("data/Plugins.txt");

	ofstream effectsFile("data/Effects.txt");
	for (const auto& effect : config.magicalEffectsList)
	{
		effectsFile << effect.name << endl;
		effectsFile << hex << uppercase << effect.id << dec << " ";
		effectsFile << hex << uppercase << effect.flags << " ";
		effectsFile << dec << effect.baseCost << endl;
		effectsFile << effect.description << endl;
	}
	effectsFile.close();

	ofstream ingredientsFile("data/Ingredients.txt");
	for (const auto& ingr : config.ingredientsList)
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
	ingredientsFile.close();
}

void loadMods(modParser::Config& config)
{
	ifstream pathsFile("data/paths.txt");
	string modPath;
	while (getline(pathsFile, modPath))
		modParser::Mod::parse(modPath, config);
}

int main(int argc, char** argv)
{
	modParser::Config config;

	loadMods(config);
	exportConfig(config);

	// Returns 0 only if the 3 lists are not empty
	return (config.modsList.empty() 
		|| config.magicalEffectsList.empty() 
		|| config.ingredientsList.empty())
		? 1 : 0;
}
