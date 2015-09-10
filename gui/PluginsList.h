#ifndef PLUGINSLIST_H
#define PLUGINSLIST_H

#include <QString>
#include <QVector>

class PluginsList
{
public:
	static PluginsList& instance();

	struct Plugin
	{
		QString name;
		bool active = true;
		int nbIngredients = 0;
	};

	int size() const;
	int find(const QString& name) const; // -1 if not found
	const QVector<Plugin>& plugins() const;
	QVector<Plugin>& plugins();

	void loadList();
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

inline QVector<PluginsList::Plugin>& PluginsList::plugins()
{ return m_plugins; }

#endif // PLUGINSLIST_H
