#pragma once

#include "Mod.h"
#include "Config.h"

namespace modParser
{

class ConfigParser : public Mod
{
public:
	static void parse(const std::string& fileName, const std::string& language, Config& config);
	
protected:
	ConfigParser(const std::string& fileName, const std::string& language, Config& config);

	bool setIngredient(const Ingredient& ingredient); // Return true if adding, false if modifying existing
	void updateMagicalEffects();

	bool m_parsedMGEF = false;
	int m_nbIngrAdded = 0, m_nbIngrModified = 0,
		m_nbEffAdded = 0, m_nbEffModified = 0;

	Ingredient m_currentIngredient;
	MagicalEffect m_currentMagicalEffect;
	Config& m_config;

	struct MGEFEntry
	{
		MGEFEntry() = default;
		MGEFEntry(uint32_t id, uint32_t dataSize, std::streamoff offset)
			: id(id), dataSize(dataSize), offset(offset) {}

		uint32_t id, dataSize;
		std::streamoff offset;
	};
	std::vector<MGEFEntry> m_magicalEffectsOffsets;

	struct MGEFEntryComp
	{
		uint32_t idOf(const MGEFEntry& t) { return t.id; }
		uint32_t idOf(const uint32_t& t) { return t; }

		template <class T, class U>
		bool operator() (const T& lhs, const U& rhs) { return idOf(lhs) < idOf(rhs); }
	};
};

} // namespace modParser
