#include <QFile>
#include <QTextStream>

#include "EffectsList.h"

const QString fileName = "data/effects.txt";

void EffectsList::loadList()
{
	m_effects.clear();

	QFile inputFile(fileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Effect effect;
			effect.name = in.readLine();

			QString line = in.readLine();
			QStringList split = line.split(" ");
			if (split.size() == 3)
			{
				effect.code = split[0].toUInt(nullptr, 16);
				effect.flags = split[1].toUInt(nullptr, 16);
				effect.baseCost = split[2].toFloat();
			}

			effect.description = in.readLine();
			if (!effect.name.isEmpty() && effect.code)
				m_effects.push_back(effect);
		}
		inputFile.close();
	}

	// Sort the list by name
	std::sort(m_effects.begin(), m_effects.end(), [](const Effect& lhs, const Effect& rhs){
		return lhs.name < rhs.name;
	});
}

void EffectsList::saveList()
{
	QFile outputFile(fileName);
	if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for(const Effect& effect : m_effects)
		{
			out << effect.name << '\n';
			out << QString::number(effect.code, 16).toUpper() << ' ';
			out << QString::number(effect.flags, 16).toUpper() << ' ';
			out << effect.baseCost << '\n';
			out << effect.description << '\n';
		}
	}
}

int EffectsList::find(quint32 code) const
{
	auto it = std::find_if(m_effects.begin(), m_effects.end(), [code](const Effect& effect){
		return effect.code == code;
	});
	if(it != m_effects.end())
		return it - m_effects.begin();
	return -1;
}

int EffectsList::find(QString name) const
{
	auto it = std::find_if(m_effects.begin(), m_effects.end(), [&name](const Effect& effect){
		return !effect.name.compare(name, Qt::CaseInsensitive);
	});
	if(it != m_effects.end())
		return it - m_effects.begin();
	return -1;
}
