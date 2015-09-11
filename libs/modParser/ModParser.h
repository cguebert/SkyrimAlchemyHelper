#include "Config.h"

namespace modParser
{

class ModParser
{
public:
	using StringsList = std::vector<std::string>;
	void setModsList(const StringsList& list); // Set all mods that will be parsed

	Config parseConfig(); // Parse all mods and look for alchemy ingredients and their effects
	void exportConfig(const Config& config); // Useful for debug purposes

protected:
	StringsList m_modsList;
};

} // namespace modParser
