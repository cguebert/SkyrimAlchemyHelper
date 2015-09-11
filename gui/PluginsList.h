#ifndef PLUGINSLIST_H
#define PLUGINSLIST_H

#include <QString>
#include <vector>

class PluginsList
{
public:
	static PluginsList& instance() // Not a singleton, just the one everyone use
	{ static PluginsList plugins; return plugins; }

	struct Plugin
	{
		Plugin(QString name = "") : name(name) {}
		int nbIngredients = 0;
		QString name;
	};
	using Plugins = std::vector<Plugin>;

	int size() const;
	int find(const QString& name) const; // -1 if not found
	const Plugins& plugins() const;
	Plugins& plugins();

	void loadList();
	void saveList();

protected:
	std::vector<Plugin> m_plugins;
};

//****************************************************************************//

inline int PluginsList::size() const
{ return m_plugins.size(); }

inline const PluginsList::Plugins& PluginsList::plugins() const
{ return m_plugins; }

inline PluginsList::Plugins& PluginsList::plugins()
{ return m_plugins; }

#endif // PLUGINSLIST_H
