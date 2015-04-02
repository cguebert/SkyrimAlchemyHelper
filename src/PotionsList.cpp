#include <QFile>
#include <QTextStream>

#include "PotionsList.h"

PotionsList& PotionsList::GetInstance()
{
	static PotionsList potions;
	return potions;
}

PotionsList::PotionsList()
{

}
