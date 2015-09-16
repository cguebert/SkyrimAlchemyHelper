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

void ModParser::setLanguage(const string& language)
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

	// Split the ids list by mod
	struct ModData
	{
		int8_t id;
		shared_ptr<ContainersParser> parser;
		vector<uint32_t> ids, ids2;
	};
	vector<ModData> mods;
	int nbMods = m_modsList.size();
	mods.resize(nbMods);
	for (int i = 0; i < nbMods; ++i)
		mods[i].id = i;

	for (auto id : ids)
	{
		int8_t modId = id >> 24;
		mods[modId].ids.push_back(id);
	}

	// Ask the information for each container and put it in this list
	ContainersParser::Containers containers;
	for (auto& mod : mods)
	{
		if (mod.ids.empty())
			continue;

		if (!mod.parser)
			mod.parser = make_shared<ContainersParser>(m_modsList[mod.id], m_language, modsNames);

		auto c = mod.parser->findContainers(mod.ids);
		containers.insert(containers.end(), c.begin(), c.end());
		mod.ids.clear();
	}

	for (const auto& c : containers)
		cout << hex << uppercase << c.id << " " << c.base << " " << c.cell << dec << endl;
	
	// Make a list of the cells
	vector<uint32_t> cells;
	for (const auto& c : containers)
		cells.push_back(c.cell);

	sort(cells.begin(), cells.end());
	cells.erase(unique(cells.begin(), cells.end()), cells.end());

	// Split by mod 
	for (auto c : cells)
	{
		int8_t modId = c >> 24;
		auto& mod = mods[modId];
		mod.ids.push_back(c);
	}

	// Ask the information for each cell
	for (auto& mod : mods)
	{ 
		if (mod.ids.empty())
			continue;

		if (!mod.parser)
			mod.parser = make_shared<ContainersParser>(m_modsList[mod.id], m_language, modsNames);

		mod.parser->getCellsName(mod.ids);
		mod.ids.clear();
	}
}

} // namespace modParser