#include "Mod.h"

#include <iostream>
#include <fstream>

namespace modParser
{

using namespace std;

string getModName(const string& modFileName)
{
	auto p = modFileName.find_last_of("/\\");
	if (p != string::npos)
		++p;
	else
		p = 0;
	return modFileName.substr(p);
}

void Mod::parse(const string& fileName, Config& config)
{
	Mod mod(fileName, config);
	mod.doParse();

	if (mod.m_nbEffAdded || mod.m_nbEffModified)
		cout << mod.m_modName << ": Effects -> added: " << mod.m_nbEffAdded << ", modified: " << mod.m_nbEffModified << endl;

	if (mod.m_nbIngrAdded || mod.m_nbIngrModified)
	{
		cout << mod.m_modName << ": Ingredients -> added: " << mod.m_nbIngrAdded << ", modified: " << mod.m_nbIngrModified << endl;

		if (mod.m_nbIngrAdded)
			config.modsList.emplace_back(mod.m_modName);
	}
}

Mod::Mod(const string& fileName, Config& config)
	: m_modFileName(fileName)
	, m_currentRecord(RecordType::None)
	, m_config(config)
{
	ifstream stream(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cerr << "Cannot open " << fileName << endl;
		return;
	}

	in.setStream(move(stream));

	m_modName = getModName(fileName);
}

void Mod::doParse()
{
	while (!in.eof())
		parseRecord();
	updateMagicalEffects(); // Get the names of the magical effects used in the ingredients
}

bool Mod::isType(const Type& type, const string& name)
{
	if (name.size() != 4)
		return false;

	for (int i = 0; i < 4; ++i)
	{
		if (type[i] != name[i])
			return false;
	}

	return true;
}

void Mod::newIngredient()
{
	m_currentIngredient.id = 0;
	m_currentIngredient.name.clear();
	m_currentIngredient.modName.clear();
	for (auto& effect : m_currentIngredient.effects)
	{
		effect.id = effect.duration = 0;
		effect.magnitude = 0;
	}
}

void Mod::newMagicalEffect()
{
	m_currentMagicalEffect.id = 0;
	m_currentMagicalEffect.name.clear();
	m_currentMagicalEffect.flags = 0;
	m_currentMagicalEffect.baseCost = 0;
}

void Mod::parseRecord()
{
	Type type{};
	in >> type;

	if (in.stream().eof())
		return;

	if (isType(type, "INGR"))
		parseIngredient();
	else if (isType(type, "MGEF"))
		parseMagicalEffect();
	else if (isType(type, "GRUP"))
		parseGroup();
	else if (isType(type, "TES4"))
		parsePluginInformation();
	else
		parseGenericRecord();
}

void Mod::parseGenericRecord()
{
	uint32_t dataSize;
	in >> dataSize;
	in.jump(dataSize + 16);
}

void Mod::parsePluginInformation()
{
	uint32_t dataSize, flags;
	in >> dataSize >> flags;
	in.jump(12);

	m_useStringsTable = (flags & 0x80) != 0;
	if (m_useStringsTable)
		m_stringsTable.load(m_modFileName);

	auto start = in.tellg();
	m_currentRecord = RecordType::Plugin;
	while (in.tellg() - start < dataSize)
		parseField();
	m_currentRecord = RecordType::None;
}

void Mod::parseGroup()
{
	auto start = in.tellg() - 4;
	uint32_t groupSize;
	Type label;
	in >> groupSize >> label;
	in.jump(12);

	if (isType(label, "INGR") || isType(label, "MGEF"))
	{
		while (in.tellg() - start < groupSize)
			parseRecord();
	}
	else
		in.seekg(start + groupSize);
}

bool Mod::setIngredient(const Ingredient& ingredient)
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

void Mod::parseIngredient()
{
	newIngredient();

	uint32_t dataSize, id;
	in >> dataSize; in.jump(4);
	in >> id; in.jump(8);

	computeIngredientId(id);

	auto start = in.tellg();
	m_currentRecord = RecordType::Ingredient;
	while (in.tellg() - start < dataSize)
		parseField();
	m_currentRecord = RecordType::None;

	if (setIngredient(m_currentIngredient))
		++m_nbIngrAdded;
	else
		++m_nbIngrModified;
}

void Mod::parseMagicalEffect()
{
	newMagicalEffect();

	uint32_t dataSize, id;
	in >> dataSize; in.jump(4);
	in >> id; in.jump(8);

	auto offset = in.tellg();
	in.jump(dataSize);
	m_magicalEffectsOffsets.emplace_back(id, offset);
}

void Mod::parseField()
{
	Type type{};
	in >> type;

	switch (m_currentRecord)
	{
	case RecordType::Plugin:
		if (isType(type, "MAST"))
			parseMaster();
		else
			parseGenericField();
		break;

	case RecordType::Ingredient:
		if (isType(type, "FULL"))
			m_currentIngredient.name = readLStringField();
		else if (isType(type, "EFID"))
			parseEffectID();
		else if (isType(type, "EFIT"))
			parseEffectItem();
		else
			parseGenericField();
		break;

	case RecordType::MagicalEffect:
		if (isType(type, "FULL"))
			m_currentMagicalEffect.name = readLStringField();
		else if (isType(type, "DATA"))
			parseMagicalEffectData();
		else if (isType(type, "DNAM"))
		{
			m_currentMagicalEffect.description = readLStringField();
			m_parsedMGEF = true;
		}
		else
			parseGenericField();
		break;

	default:
		parseGenericField();
	}
}

void Mod::parseGenericField()
{
	uint16_t dataSize;
	in >> dataSize;
	in.jump(dataSize);
}

void Mod::parseMaster()
{
	uint16_t dataSize;
	in >> dataSize;

	string name;
	name.resize(dataSize - 1); // Don't read null character in the string
	in.stream().read(&name[0], dataSize - 1);
	in.jump(1);

	m_masters.push_back(getModName(name));
}

void Mod::parseMagicalEffectData()
{
	uint16_t dataSize;
	in >> dataSize;
	in >> m_currentMagicalEffect.flags;
	in >> m_currentMagicalEffect.baseCost;
	in.jump(144); // DATA is 152 bytes long
}

void Mod::parseEffectID()
{
	uint16_t dataSize;
	in >> dataSize;

	uint32_t id;
	in >> id;

	for (auto& effect : m_currentIngredient.effects)
	{
		if (!effect.id)
		{
			effect.id = id;
			break;
		}
	}
}

void Mod::parseEffectItem()
{
	uint16_t dataSize;
	in >> dataSize;

	uint32_t area;
	int i = 0;
	while (i < 3 && m_currentIngredient.effects[i + 1].id)
		++i;
	in >> m_currentIngredient.effects[i].magnitude >> area >> m_currentIngredient.effects[i].duration;
}

void Mod::computeIngredientId(uint32_t id)
{
	uint8_t modId = id >> 24;
	m_currentIngredient.id = id & 0x00FFFFFF;

	unsigned int nbMasters = m_masters.size();
	if (modId == nbMasters) // Introduced by this mod
		m_currentIngredient.modName = m_modName;
	else if (modId < nbMasters)
		m_currentIngredient.modName = m_masters[modId];
	else
		cerr << "Error: invalid modId " << hex << (int)modId << endl;
}

void Mod::updateMagicalEffects()
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
		return lhs.first < rhs.first;
	});

	// Find the intersection of the 2 lists
	vector<MGEFEntry> effectsToUpdate;
	set_intersection(m_magicalEffectsOffsets.begin(), m_magicalEffectsOffsets.end(),
		effectsIds.begin(), effectsIds.end(), back_inserter(effectsToUpdate), MGEFEntryComp());

	// Get the name of each magical effect
	MagicalEffectsList updatedEffects;
	m_currentRecord = RecordType::MagicalEffect;
	for (const auto& effect : effectsToUpdate)
	{
		newMagicalEffect();
		m_currentMagicalEffect.id = effect.first;
		m_parsedMGEF = false;
		in.seekg(effect.second);

		while (!m_parsedMGEF && !in.eof())
			parseField();

		if (!m_currentMagicalEffect.name.empty())
			updatedEffects.push_back(m_currentMagicalEffect);
	}
	m_currentRecord = RecordType::None;

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

string Mod::readLStringField()
{
	uint16_t dataSize;
	in >> dataSize;

	if (m_useStringsTable)
	{
		uint32_t id;
		in >> id;
		return m_stringsTable.get(id);
	}
	else
	{
		string text;
		text.resize(dataSize - 1); // Don't read null character in the string
		in.stream().read(&text[0], dataSize - 1);
		in.jump(1);
		return text;
	}
}

} // namespace modParser