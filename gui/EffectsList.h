#ifndef EFFECTSLIST_H
#define EFFECTSLIST_H

#include <QString>
#include <vector>

class EffectsList
{
public:
	static EffectsList& instance() // Not a singleton, just the one everyone use
	{ static EffectsList effects; return effects; }

	struct Effect
	{
		quint32 code = 0, flags = 0;
		float baseCost = 0;
		QString name, description, tooltip;
		std::vector<int> ingredients;
	};
	using Effects = std::vector<Effect>;

	int size() const;
	int find(quint32 code) const; // -1 if not found
	int find(QString name) const; // -1 if not found
	const Effects& effects() const;
	Effects& effects();

	void loadList();
	void saveList();

protected:
	Effects m_effects;
};

//****************************************************************************//

inline int EffectsList::size() const
{ return m_effects.size(); }

inline const EffectsList::Effects& EffectsList::effects() const
{ return m_effects; }

inline EffectsList::Effects& EffectsList::effects()
{ return m_effects; }

#endif // EFFECTSLIST_H
