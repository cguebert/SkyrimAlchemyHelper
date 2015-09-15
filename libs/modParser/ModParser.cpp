#include <iostream>
#include <fstream>

#include "ModParser.h"
#include "ConfigParser.h"

namespace modParser
{

using namespace std;

void ModParser::setModsList(const StringsList& list)
{
	m_modsList = list;
}

void ModParser::setLanguage(const std::string& language)
{
	m_language = language;
}

Config ModParser::parseConfig()
{
	Config config;
	for (const auto& modPath : m_modsList)
		ConfigParser::parse(modPath, m_language, config);
	return config;
}

float significant(float v, int d)
{
	float p = pow(10.0f, d);
	return round(v * p) / p;
}

void ModParser::exportConfig(const Config& config)
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
			ingredientsFile << significant(effect.magnitude, 6) << " ";
			ingredientsFile << effect.duration << endl;
		}
	}
	ingredientsFile.close();
}

} // namespace modParser