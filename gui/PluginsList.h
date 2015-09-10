#ifndef PLUGINSLIST_H
#define PLUGINSLIST_H

#include <QString>
#include <vector>

class PluginsList
{
public:
	static PluginsList& instance();

	struct Plugin
	{
		int nbIngredients = 0;
		QString name;
	};

	int size() const;
	int find(const QString& name) const; // -1 if not found
	const std::vector<Plugin>& plugins() const;
	std::vector<Plugin>& plugins();

	void loadList();
	void saveList();

protected:
	PluginsList();

	std::vector<Plugin> m_plugins;
};

//****************************************************************************//

inline int PluginsList::size() const
{ return m_plugins.size(); }

inline const std::vector<PluginsList::Plugin>& PluginsList::plugins() const
{ return m_plugins; }

inline std::vector<PluginsList::Plugin>& PluginsList::plugins()
{ return m_plugins; }

#endif // PLUGINSLIST_H
