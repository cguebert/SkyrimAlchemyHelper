/******************************************************************************
*                            Skyrim Alchemy Helper                            *
*******************************************************************************
*                                                                             *
* Copyright (C) 2015 Christophe Guebert                                       *
*                                                                             *
* This program is free software; you can redistribute it and/or modify        *
* it under the terms of the GNU General Public License as published by        *
* the Free Software Foundation; either version 2 of the License, or           *
* (at your option) any later version.                                         *
*                                                                             *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc.,     *
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.                 *
*******************************************************************************
*                      Skyrim Alchemy Helper :: ModParser                     *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
#include "ConfigParser.h"

#include <iostream>
#include <fstream>

namespace modParser
{

using namespace std;

void ConfigParser::parse(const std::string& fileName, const std::string& language, Config& config)
{
	ConfigParser mod(fileName, language, config);
	mod.doParse();
	mod.updateMagicalEffects();

	if (mod.m_nbEffAdded || mod.m_nbEffModified)
		cout << mod.m_modName << ": Effects -> added: " << mod.m_nbEffAdded << ", modified: " << mod.m_nbEffModified << endl;

	if (mod.m_nbIngrAdded || mod.m_nbIngrModified)
	{
		cout << mod.m_modName << ": Ingredients -> added: " << mod.m_nbIngrAdded << ", modified: " << mod.m_nbIngrModified << endl;

		if (mod.m_nbIngrAdded)
			config.modsList.emplace_back(mod.m_modName);
	}
}

ConfigParser::ConfigParser(const std::string& fileName, const std::string& language, Config& config)
	: Mod(fileName, language)
	, m_config(config)
{
// Ingredients
	RecordParser ingRecord;
	ingRecord.type = "INGR";
	ingRecord.beginFunction = [this](uint32_t id, uint32_t /*dataSize*/, uint32_t /*flags*/) {
		m_currentIngredient.id = id & 0x00FFFFFF;
		m_currentIngredient.modName = getMaster(id);
		m_currentIngredient.name.clear();
		for (auto& effect : m_currentIngredient.effects)
		{
			effect.id = effect.duration = 0;
			effect.magnitude = 0;
		}
		return true;
	};

	ingRecord.endFunction = [this](uint32_t id) {
		if (setIngredient(m_currentIngredient))
			++m_nbIngrAdded;
		else
			++m_nbIngrModified;
	};

	ingRecord.fields.emplace_back("FULL", [this](uint16_t dataSize) {
		m_currentIngredient.name = readLStringField(dataSize);
	});

	ingRecord.fields.emplace_back("EFID", [this](uint16_t dataSize) {
		uint32_t id; in >> id;
		for (auto& effect : m_currentIngredient.effects)
		{
			if (!effect.id)
			{
				effect.id = id;
				break;
			}
		}
	});

	ingRecord.fields.emplace_back("EFIT", [this](uint16_t dataSize) {
		uint32_t area;
		int i = 0;
		while (i < 3 && m_currentIngredient.effects[i + 1].id)
			++i;
		in >> m_currentIngredient.effects[i].magnitude >> area >> m_currentIngredient.effects[i].duration;
	});

	m_groupParsers.emplace_back("INGR", RecordParsers({ ingRecord } ));

// Magical effects
	// We don't parse every effects yet, we will filter later for only the alchemy ones
	RecordParsers mgefParsers = { { "MGEF", {}, [this](uint32_t id, uint32_t dataSize, uint32_t /*flags*/) {
		m_magicalEffectsOffsets.emplace_back(id, dataSize, in.tellg());
		return false;
	} } };

	m_groupParsers.emplace_back("MGEF", mgefParsers);
}

bool ConfigParser::setIngredient(const Ingredient& ingredient)
{
	auto it = std::find_if(m_config.ingredientsList.begin(), m_config.ingredientsList.end(), [&ingredient](const Ingredient& ing){
		return ingredient.id == ing.id && ingredient.modName == ing.modName;
	});

	if (it != m_config.ingredientsList.end())
	{
		*it = ingredient;
		return false; // false if modifying
	}
	else
	{
		m_config.ingredientsList.push_back(ingredient);
		return true; // true if adding
	}
}

void ConfigParser::updateMagicalEffects()
{
	if (m_magicalEffectsOffsets.empty())
		return;

	in.stream().clear();

	// Compute the list of ids used by the ingredients
	vector<uint32_t> effectsIds;
	effectsIds.reserve(m_config.ingredientsList.size() * 4);
	for (const auto& ing : m_config.ingredientsList)
	{
		for (const auto& eff : ing.effects)
			effectsIds.push_back(eff.id);
	}

	// Sort the effects list used by the ingredients
	sort(effectsIds.begin(), effectsIds.end());
	auto last = unique(effectsIds.begin(), effectsIds.end());
	effectsIds.erase(last, effectsIds.end());

	// Sort the magical effects list of this mod
	sort(m_magicalEffectsOffsets.begin(), m_magicalEffectsOffsets.end(), [](const MGEFEntry& lhs, const MGEFEntry& rhs){
		return lhs.id < rhs.id;
	});

	// Find the intersection of the 2 lists
	vector<MGEFEntry> effectsToUpdate;
	set_intersection(m_magicalEffectsOffsets.begin(), m_magicalEffectsOffsets.end(),
		effectsIds.begin(), effectsIds.end(), back_inserter(effectsToUpdate), MGEFEntryComp());

	// Prepare the parsers for the magical effects fields
	FieldParsers mgefFields;
	mgefFields.emplace_back("FULL", [this](uint16_t dataSize) {
		m_currentMagicalEffect.name = readLStringField(dataSize);
	});

	mgefFields.emplace_back("DATA", [this](uint16_t dataSize) {
		in >> m_currentMagicalEffect.flags >> m_currentMagicalEffect.baseCost;
		in.jump(144); // DATA is 152 bytes long
	});

	mgefFields.emplace_back("DNAM", [this](uint16_t dataSize) {
		m_currentMagicalEffect.description = readLStringField(dataSize);
	});

	// Get the name of each magical effect
	MagicalEffectsList updatedEffects;
	for (const auto& effect : effectsToUpdate)
	{
		m_currentMagicalEffect.id = effect.id;
		m_currentMagicalEffect.name.clear();
		m_currentMagicalEffect.flags = 0;
		m_currentMagicalEffect.baseCost = 0;
		
		in.seekg(effect.offset);
		parseFields(mgefFields, effect.dataSize);

		if (!m_currentMagicalEffect.name.empty())
			updatedEffects.push_back(m_currentMagicalEffect);
	}

	// Merge the updated effects into the main list
	MagicalEffectsList outputList;
	set_union(updatedEffects.begin(), updatedEffects.end(),
		m_config.magicalEffectsList.begin(), m_config.magicalEffectsList.end(),
		back_inserter(outputList), [](const MagicalEffect& lhs, const MagicalEffect& rhs){
		return lhs.id < rhs.id;
	});

	// Compute the number of effects added or modified
	int oldSize = m_config.magicalEffectsList.size();
	int updateSize = updatedEffects.size();
	int newSize = outputList.size();
	m_nbEffAdded = newSize - oldSize;
	m_nbEffModified = updateSize - m_nbEffAdded;

	m_config.magicalEffectsList.swap(outputList);
}

} // namespace modParser
