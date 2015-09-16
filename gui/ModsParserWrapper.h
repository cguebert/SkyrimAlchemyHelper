#pragma once

#include <QString>

#include "Config.h"
#include <modParser/ModParser.h>

class ModsParserWrapper
{
public:
	ModsParserWrapper(); // Using Settings values
	ModsParserWrapper(bool useModOrganizer,
		QString dataFolder,
		QString pluginsListPath,
		QString modOrganizerPath,
		QString language);

	enum class Result { Success, Error_ModOrganizer, Error_ModsParsing };
	Result parseConfig();
	void copyToConfig(Config& config) const;

	Result updateContainers(const std::vector<uint32_t>& ids);
	const ContainersCache::Containers& containers() const;

	int nbPlugins() const;
	int nbIngredients() const;
	int nbEffects() const;

protected:
	bool getModsPaths(std::vector<std::string>& paths);
	bool findRealPaths(std::vector<std::string>& paths);

	bool m_useModOrganizer = false;
	QString m_dataFolder, m_pluginsListPath, m_modOrganizerPath, m_language = "english";

	modParser::ModParser m_parser;
	modParser::Config m_config;
	ContainersCache::Containers m_containers;
};

//****************************************************************************//

inline int ModsParserWrapper::nbPlugins() const
{ return m_config.modsList.size(); }

inline int ModsParserWrapper::nbIngredients() const
{ return m_config.ingredientsList.size(); }

inline int ModsParserWrapper::nbEffects() const
{ return m_config.magicalEffectsList.size(); }

inline const ContainersCache::Containers& ModsParserWrapper::containers() const
{ return m_containers; }