#include "Config.h"

namespace modParser
{

class ModParser
{
public:
	using StringsList = std::vector<std::string>;
	void setModsList(const StringsList& list); // Set all mods that will be parsed
	void setLanguage(const std::string& language);

	Config parseConfig(); // Parse all mods and look for alchemy ingredients and their effects
	void exportConfig(const Config& config); // Useful for debug purposes

	struct Container { uint32_t id; std::string type, location; };
	using Containers = std::vector<Container>;
	using IdsList = std::vector<uint32_t>;
	Containers getContainersInfo(const IdsList& ids);

protected:
	StringsList m_modsList;
	std::string m_language = "english";
};

} // namespace modParser
