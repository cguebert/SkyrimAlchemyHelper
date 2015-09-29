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

#include "PotionsList.h"

#include <array>

class DiscoverEffects
{
public:
	static std::pair<PotionsList::PotionsId, PotionsList::PotionAdditionalDataList> selectPotions(int nb = 50);

protected:
	DiscoverEffects(int nbPotions);
	void doComputation();

	void setFilters();
	void setSortingFunction();
	bool selectOnePotion();

	void prepare();
	void cleanup();

	int m_nbPotions;
	std::vector<int> m_ingredientsCount;
	std::vector<std::array<bool, 4>> m_knownIngredients;
	PotionsList::PotionsId m_selectedPotions;
	PotionsList::PotionAdditionalDataList m_additionalData;
};
