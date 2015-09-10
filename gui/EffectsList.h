#ifndef EFFECTSLIST_H
#define EFFECTSLIST_H

#include <QString>
#include <QVector>

class EffectsList
{
public:
	static EffectsList& instance();

	struct Effect
	{
		QString name;
		quint32 code = 0;
		quint32 flags = 0;
		float baseCost = 0;
		std::vector<int> ingredients;
	};

	int size() const;
	int find(quint32 code) const; // -1 if not found
	int find(QString name) const; // -1 if not found
	const QVector<Effect>& effects() const;
	QVector<Effect>& effects();

	void loadList();
	void saveList();

protected:
	EffectsList();

	QVector<Effect> m_effects;
};

//****************************************************************************//

inline int EffectsList::size() const
{ return m_effects.size(); }

inline const QVector<EffectsList::Effect>& EffectsList::effects() const
{ return m_effects; }

inline QVector<EffectsList::Effect>& EffectsList::effects()
{ return m_effects; }

#endif // EFFECTSLIST_H
