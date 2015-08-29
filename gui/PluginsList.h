#ifndef PLUGINSLIST_H
#define PLUGINSLIST_H

#include <QString>
#include <QVector>

class PluginsList
{
public:
	static PluginsList& GetInstance();

	struct Plugin
	{
		Plugin() : active(true), nbIngredients(0) {}
		QString name;
		bool active;
		int nbIngredients;
	};

	int size() const;
	int find(const QString& name) const; // -1 if not found
	const QVector<Plugin>& plugins() const;
	Plugin& plugin(int index); // Will launch an exception if index is invalid
	const Plugin& plugin(int index) const; // Will launch an exception if index is invalid

	void setActive(int index, bool active);
	void incrementNbIngredients(int index);
	void decrementNbIngredients(int index);

	void removePlugins(int first, int count = 1);
	void addPlugin();

	void saveList();

protected:
	PluginsList();

	QVector<Plugin> m_plugins;
};

//****************************************************************************//

inline int PluginsList::size() const
{ return m_plugins.size(); }

inline const QVector<PluginsList::Plugin>& PluginsList::plugins() const
{ return m_plugins; }

inline PluginsList::Plugin& PluginsList::plugin(int index)
{ return m_plugins[index]; }

inline const PluginsList::Plugin& PluginsList::plugin(int index) const
{ return m_plugins[index]; }

inline void PluginsList::setActive(int index, bool active)
{ m_plugins[index].active = active; }

inline void PluginsList::incrementNbIngredients(int index)
{ ++m_plugins[index].nbIngredients; }

inline void PluginsList::decrementNbIngredients(int index)
{ --m_plugins[index].nbIngredients; }

#endif // PLUGINSLIST_H
