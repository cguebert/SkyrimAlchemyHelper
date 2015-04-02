#ifndef POTIONSLIST_H
#define POTIONSLIST_H

#include <QString>
#include <QVector>

class PotionsList
{
public:
	static PotionsList& GetInstance();

protected:
	PotionsList();
};

//****************************************************************************//



#endif // POTIONSLIST_H
