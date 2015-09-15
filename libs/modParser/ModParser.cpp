#include <iostream>
#include <fstream>
#include <memory>

#include "ModParser.h"
#include "ConfigParser.h"
#include "ContainersParser.h"

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

void ModParser::getContainersInfo(const IdsList& ids)
{
	// Create a list of all mods names
	StringsList modsNames;
	for (const auto& path : m_modsList)
		modsNames.push_back(Mod::getModName(path));

	// Sort the ids list by mod
	struct ModData
	{
		shared_ptr<ContainersParser> parser;
		vector<uint32_t> containersIds;
	};
	vector<ModData> mods;
	int nbMods = m_modsList.size();
	mods.resize(nbMods);

	for (auto id : ids)
	{
		int8_t modId = id >> 24;
		auto& mod = mods[modId];
		if (!mod.parser)
			mod.parser = make_shared<ContainersParser>(m_modsList[modId], m_language, modsNames);
		mod.containersIds.push_back(id);
	}

	// Ask the information for each container and put it in this list
	ContainersParser::Containers containers;
	for (auto& mod : mods)
	{
		if (mod.parser)
		{
			auto c = mod.parser->findContainers(mod.containersIds);
			containers.insert(containers.end(), c.begin(), c.end());
		}
	}

	for (const auto& c : containers)
		std::cout << hex << uppercase << c.id << " " << c.base << " " << c.cell << dec << endl;
}

} // namespace modParser