#include <QFile>
#include <QTextStream>

#include "EffectsList.h"

const QString fileName = "data/effects.txt";

EffectsList& EffectsList::instance()
{
	static EffectsList effects;
	return effects;
}

EffectsList::EffectsList()
{
	loadList();
}

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
			effect.id = line.toUInt(nullptr, 16);
			if (!effect.name.isEmpty() && effect.id)
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
			out << QString::number(effect.id, 16).toUpper() << '\n';
		}
	}
}

int EffectsList::find(quint32 id) const
{
	auto it = std::find_if(m_effects.begin(), m_effects.end(), [id](const Effect& effect){
		return effect.id == id;
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

void EffectsList::removeEffects(int first, int count)
{
	auto start = m_effects.begin() + first;
	auto end = start + count;
	m_effects.erase(start, end);
}

void EffectsList::addEffect()
{
	Effect effect;
	effect.name = "New Effect";
	m_effects.push_back(effect);
}
