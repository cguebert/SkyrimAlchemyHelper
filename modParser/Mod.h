#pragma once

#include <parser/Parser.h>
#include "Config.h"
#include "StringsTable.h"

class Mod
{
public:
	static void parse(const std::string& fileName, Config& config);
	
protected:
	Mod(const std::string& fileName, Config& config);
	void doParse();

	using Type = std::array<char, 4>;
	bool isType(const Type& type, const std::string& name); 

	void newIngredient();
	void newMagicalEffect();
	
	void parseRecord();
	void parseGenericRecord();
	void parsePluginInformation();
	void parseGroup();
	void parseIngredient();
	void parseMagicalEffect();

	void parseField();
	void parseGenericField();
	void parseMaster();
	void parseMagicalEffectData();
	void parseEffectID();
	void parseEffectItem();

	void computeIngredientId(uint32_t id);
	bool setIngredient(const Ingredient& ingredient); // Return true if adding, false if modifying existing
	void updateMagicalEffects();

	std::string readLStringField();
	
	bool m_useStringsTable = false,
		m_parsedMGEF = false;
	int m_nbIngrAdded = 0, m_nbIngrModified = 0,
		m_nbEffAdded = 0, m_nbEffModified = 0;

	Parser in;
	std::string m_modFileName, m_modName;
	Ingredient m_currentIngredient;
	MagicalEffect m_currentMagicalEffect;
	enum class RecordType { None, Plugin, Ingredient, MagicalEffect };
	RecordType m_currentRecord;
	Config& m_config;
	StringsTable m_stringsTable;
	std::vector<std::string> m_masters;

	using MGEFEntry = std::pair < uint32_t, std::streamoff >;
	std::vector<MGEFEntry> m_magicalEffectsOffsets;

	struct MGEFEntryComp
	{
		uint32_t idOf(const MGEFEntry& t) { return t.first; }
		uint32_t idOf(const uint32_t& t) { return t; }

		template <class T, class U>
		bool operator() (const T& lhs, const U& rhs) { return idOf(lhs) < idOf(rhs); }
	};
};

