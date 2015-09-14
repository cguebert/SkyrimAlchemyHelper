#pragma once

#include <QString>

#include "Config.h"
#include <modParser/ModParser.h>

class ConfigModsParser
{
public:
	ConfigModsParser(bool useModOrganizer,
		QString dataFolder,
		QString pluginsListPath,
		QString modOrganizerPath,
		QString language);
	enum class Result { Success, Error_ModOrganizer, Error_ModsParsing };
	Result parse();
	void copyToConfig(Config& config);

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
};

//****************************************************************************//

inline int ConfigModsParser::nbPlugins() const
{ return m_config.modsList.size(); }

inline int ConfigModsParser::nbIngredients() const
{ return m_config.ingredientsList.size(); }

inline int ConfigModsParser::nbEffects() const
{ return m_config.magicalEffectsList.size(); }
