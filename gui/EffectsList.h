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
		int nbIngredients = 0;
	};

	int size() const;
	int find(quint32 code) const; // -1 if not found
	int find(QString name) const; // -1 if not found
	const QVector<Effect>& effects() const;
	Effect& effect(int index); // Will launch an exception if index is invalid
	const Effect& effect(int index) const; // Will launch an exception if index is invalid

	void setNbIngredients(int index, int nb);
	void incrementNbIngredients(int index);
	void decrementNbIngredients(int index);

	void removeEffects(int first, int count = 1);
	void addEffect();

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

inline EffectsList::Effect& EffectsList::effect(int index)
{ return m_effects[index]; }

inline const EffectsList::Effect& EffectsList::effect(int index) const
{ return m_effects[index]; }

inline void EffectsList::setNbIngredients(int index, int nb)
{ m_effects[index].nbIngredients = nb; }

inline void EffectsList::incrementNbIngredients(int index)
{ ++m_effects[index].nbIngredients; }

inline void EffectsList::decrementNbIngredients(int index)
{ --m_effects[index].nbIngredients; }

#endif // EFFECTSLIST_H
