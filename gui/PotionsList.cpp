#include <QFile>
#include <QTextStream>

#include "PotionsList.h"

PotionsList& PotionsList::instance()
{
	static PotionsList potions;
	return potions;
}

PotionsList::PotionsList()
{

}
