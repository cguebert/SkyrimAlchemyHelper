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
*                        Skyrim Alchemy Helper :: Gui                         *
*                                                                             *
*                  Contact: christophe.guebert+SAH@gmail.com                  *
******************************************************************************/
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
