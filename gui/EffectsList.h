#ifndef EFFECTSLIST_H
#define EFFECTSLIST_H

#include <QString>
#include <vector>

class EffectsList
{
public:
	static EffectsList& instance();

	struct Effect
	{
		quint32 code = 0, flags = 0;
		float baseCost = 0;
		QString name, description;
		std::vector<int> ingredients;
	};

	int size() const;
	int find(quint32 code) const; // -1 if not found
	int find(QString name) const; // -1 if not found
	const std::vector<Effect>& effects() const;
	std::vector<Effect>& effects();

	void loadList();
	void saveList();

protected:
	EffectsList();

	std::vector<Effect> m_effects;
};

//****************************************************************************//

inline int EffectsList::size() const
{ return m_effects.size(); }

inline const std::vector<EffectsList::Effect>& EffectsList::effects() const
{ return m_effects; }

inline std::vector<EffectsList::Effect>& EffectsList::effects()
{ return m_effects; }

#endif // EFFECTSLIST_H
