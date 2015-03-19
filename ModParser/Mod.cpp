#include "Mod.h"

#include <iostream>
#include <fstream>

using namespace std;

std::string getModName(const std::string& modFileName)
{
	auto p = modFileName.find_last_of("/\\");
	if (p != string::npos)
		++p;
	else
		p = 0;
	string fileName;
	auto e = modFileName.find_last_of(".");
	if (e != string::npos)
		fileName = modFileName.substr(p, e - p);
	else
		fileName = modFileName.substr(p);

	return fileName;
}

void Mod::parse(const std::string& fileName, Config& config)
{
	Mod mod(fileName, config);
	mod.doParse();

	if (mod.m_nbIngrAdded || mod.m_nbIngrModified)
	{
		cout << mod.m_modName << ": Ingredients -> added=" << mod.m_nbIngrAdded << ", modified=" << mod.m_nbIngrModified << endl;

		if (mod.m_nbIngrAdded)
			config.modsList.emplace_back(mod.m_modName);
	}

	if (mod.m_nbEffAdded || mod.m_nbEffModified)
		cout << mod.m_modName << ": Effects -> added=" << mod.m_nbEffAdded << ", modified=" << mod.m_nbEffModified << endl;
}

Mod::Mod(const std::string& fileName, Config& config)
	: m_modFileName(fileName)
	, m_config(config)
{
	ifstream stream;
	stream.open(fileName, ios::binary | ios::in);
	if (!stream.is_open())
	{
		cout << "Cannot open " << fileName << endl;
		return;
	}

	in.setStream(std::move(stream));

	m_modName = getModName(fileName);
	m_currentIngredient.modName = m_modName;
}

void Mod::doParse()
{
	while (!in.eof())
		parseRecord();
	updateMagicalEffects(); // Get the names of the magical effects used in the ingredients
}

template <class T, int N>
void print(const std::array<T, N>& array)
{
	for (const auto& a : array)
		cout << a;
	cout << endl;
}

template <class T, int N>
void printHex(const std::array<T, N>& array)
{
	for (const auto& a : array)
		cout << uppercase << hex << static_cast<int>(a);
	cout << endl;
}

bool Mod::isType(const Type& type, const std::string& name)
{
	if (name.size() != 4)
		return false;

	for (int i = 0; i<4; ++i)
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
	for (auto& effect : m_currentIngredient.effects)
	{
		effect.id = effect.duration = 0;
		effect.magnitude = 0;
	}
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
	uint32_t dataSize, flags, id, revision;
	uint16_t version, unknown;
	in >> dataSize >> flags >> id >> revision >> version >> unknown;

	in.seekg(in.tellg() + dataSize);
}

void Mod::parsePluginInformation()
{
	uint32_t dataSize, flags, id, revision;
	uint16_t version, unknown;
	in >> dataSize >> flags >> id >> revision >> version >> unknown;

	m_useStringsTable = (flags & 128) != 0;
	if (m_useStringsTable)
		m_stringsTable.load(m_modFileName);

	in.seekg(in.tellg() + dataSize);
}

void Mod::parseGroup()
{
	auto start = in.tellg() - 4;
	Type label;
	uint32_t groupSize, groupType;
	uint16_t stamp, unknown, version, unknown2;
	in >> groupSize >> label >> groupType >> stamp >>
		unknown >> version >> unknown2;

	if (isType(label, "INGR") || isType(label, "MGEF"))
	{
		while (in.tellg() - start < groupSize)
			parseRecord();
	}
	else
		in.seekg(start + groupSize);
}

void Mod::parseIngredient()
{
	m_parsingIngredient = true;
	newIngredient();

	uint32_t dataSize, flags, revision;
	uint16_t version, unknown;
	in >> dataSize >> flags >> m_currentIngredient.id >> revision >> version >> unknown;

	auto start = in.tellg();
	while (in.tellg() - start < dataSize)
		parseField();

	if (m_config.ingredientsList.setIngredient(m_currentIngredient))
		++m_nbIngrAdded;
	else
		++m_nbIngrModified;
	m_parsingIngredient = false;
}

void Mod::parseMagicalEffect()
{
	uint32_t dataSize, flags, revision, id;
	uint16_t version, unknown;
	in >> dataSize >> flags >> id >> revision >> version >> unknown;

	auto offset = in.tellg();
	in.seekg(offset + dataSize);
	m_magicalEffectsOffsets.emplace_back(id, offset);
}

void Mod::parseField()
{
	Type type{};
	in >> type;

	if (m_parsingIngredient)
	{
		if (isType(type, "FULL"))
			parseIngredientName();
		else if (isType(type, "EFID"))
			parseEffectID();
		else if (isType(type, "EFIT"))
			parseEffectItem();
		else
			parseGenericField();
	}
	else if (m_parsingMagicalEffect)
	{
		if (isType(type, "FULL"))
			parseMagicalEffectName();
		else
			parseGenericField();
	}
	else
		parseGenericField();
}

void Mod::parseGenericField()
{
	uint16_t dataSize;
	in >> dataSize;

	in.seekg(in.tellg() + dataSize);
}

void Mod::parseIngredientName()
{
	uint16_t dataSize;
	in >> dataSize;

	if (m_useStringsTable)
	{
		uint32_t id;
		in >> id;
		m_currentIngredient.name = m_stringsTable.get(id);
	}
	else
	{
		std::string& name = m_currentIngredient.name;
		name.resize(dataSize-1); // Don't read null character in the string
		in.stream().read(&name[0], dataSize-1);
		uint8_t dummy; // read null character
		in >> dummy;
	}
}

void Mod::parseMagicalEffectName()
{
	uint16_t dataSize;
	in >> dataSize;

	if (m_useStringsTable)
	{
		uint32_t id;
		in >> id;
		m_currentMagicalEffectName = m_stringsTable.get(id);
	}
	else
	{
		m_currentMagicalEffectName.resize(dataSize - 1); // Don't read null character in the string
		in.stream().read(&m_currentMagicalEffectName[0], dataSize - 1);
		uint8_t dummy; // read null character
		in >> dummy;
	}

	m_parsedMGEF = true;
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
	while (i<3 && m_currentIngredient.effects[i + 1].id)
		++i;
	in >> m_currentIngredient.effects[i].magnitude >> area >> m_currentIngredient.effects[i].duration;
}

void Mod::updateMagicalEffects()
{
	if (m_magicalEffectsOffsets.empty())
		return;

	in.stream().clear();

	// Compute the list of ids used by the ingredients
	std::vector<uint32_t> effectsIds;
	effectsIds.reserve(m_config.ingredientsList.ingredients().size() * 4);
	for (const auto& ing : m_config.ingredientsList.ingredients())
	{
		for (const auto& eff : ing.effects)
			effectsIds.push_back(eff.id);
	}

	// Sort the effects list used by the ingredients
	std::sort(effectsIds.begin(), effectsIds.end());
	auto last = std::unique(effectsIds.begin(), effectsIds.end());
	effectsIds.erase(last, effectsIds.end());

	// Sort the magical effects list of this mod
	std::sort(m_magicalEffectsOffsets.begin(), m_magicalEffectsOffsets.end(), [](const MGEFEntry& lhs, const MGEFEntry& rhs){
		return lhs.first < rhs.first;
	});

	// Find the intersection of the 2 lists
	std::vector<MGEFEntry> effectsToUpdate;
	std::set_intersection(m_magicalEffectsOffsets.begin(), m_magicalEffectsOffsets.end(),
		effectsIds.begin(), effectsIds.end(), std::back_inserter(effectsToUpdate), MGEFEntryComp());

	// Get the name of each magical effect
	MagicalEffectsList updatedEffects;
	m_parsingMagicalEffect = true;
	for (const auto& effect : effectsToUpdate)
	{
		in.seekg(effect.second);
		m_parsedMGEF = false;
		m_currentMagicalEffectName = "";

		while (!m_parsedMGEF && !in.eof())
			parseField();

		if (!m_currentMagicalEffectName.empty())
			updatedEffects.emplace_back(effect.first, m_currentMagicalEffectName);
	}
	m_parsingMagicalEffect = false;

	// Merge the updated effects into the main list
	MagicalEffectsList outputList;
	std::set_union(updatedEffects.begin(), updatedEffects.end(),
		m_config.magicalEffectsList.begin(), m_config.magicalEffectsList.end(), 
		std::back_inserter(outputList), [](const MagicalEffect& lhs, const MagicalEffect& rhs){
		return lhs.first < rhs.first;
	});

	// Compute the number of effects added or modified
	int oldSize = m_config.magicalEffectsList.size();
	int updateSize = updatedEffects.size();
	int newSize = outputList.size();
	m_nbEffAdded = newSize - oldSize;
	m_nbEffModified = updateSize - m_nbEffAdded;

	m_config.magicalEffectsList.swap(outputList);
}
