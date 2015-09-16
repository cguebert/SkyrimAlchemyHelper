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

ModParser::Containers ModParser::getContainersInfo(const IdsList& ids)
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

	// Make a list of the containers type
	vector<uint32_t> containersTypes;
	for (const auto& c : containers)
		containersTypes.push_back(c.base);

	sort(containersTypes.begin(), containersTypes.end());
	containersTypes.erase(unique(containersTypes.begin(), containersTypes.end()), containersTypes.end());

	// Split by mod 
	for (auto c : containersTypes)
	{
		int8_t modId = c >> 24;
		auto& mod = mods[modId];
		mod.ids2.push_back(c);
	}

	Containers resultCont;
	resultCont.resize(ids.size());
	for (int i = 0, nb = ids.size(); i < nb; ++i)
		resultCont[i].id = ids[i];

	using IdNamePair = pair<uint32_t, string>;
	vector<IdNamePair> locations, types;

	// Ask the information for each cell
	for (auto& mod : mods)
	{ 
		if (mod.ids.empty() && mod.ids2.empty())
			continue;

		if (!mod.parser)
			mod.parser = make_shared<ContainersParser>(m_modsList[mod.id], m_language, modsNames);

		auto names = mod.parser->getNames(mod.ids, mod.ids2);
		for (int i = 0, nb = mod.ids.size(); i < nb; ++i)
			locations.emplace_back(mod.ids[i], names.first[i].name);

		for (int i = 0, nb = mod.ids2.size(); i < nb; ++i)
			types.emplace_back(mod.ids2[i], names.second[i].name);

		mod.ids.clear();
		mod.ids2.clear();
	}

	// Merge the location and type names into the containers
	for (int i = 0, nb = containers.size(); i < nb; ++i)
	{
		uint32_t locId = containers[i].cell, typeId = containers[i].base;
		resultCont[i].cell = locId;

		auto itLoc = find_if(locations.begin(), locations.end(), [locId](const IdNamePair& inp) {
			return inp.first == locId;
		});

		if (itLoc != locations.end())
			resultCont[i].location = itLoc->second;

		auto itType = find_if(types.begin(), types.end(), [typeId](const IdNamePair& inp) {
			return inp.first == typeId;
		});

		if (itType != types.end())
			resultCont[i].type = itType->second;
	}

	return resultCont;
}

} // namespace modParser