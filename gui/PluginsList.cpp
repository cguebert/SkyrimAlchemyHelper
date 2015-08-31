#include <QFile>
#include <QTextStream>

#include "PluginsList.h"

#include <fstream>

const QString fileName = "data/plugins.txt";

PluginsList& PluginsList::GetInstance()
{
	static PluginsList plugins;
	return plugins;
}

PluginsList::PluginsList()
{
	loadList();
}

void PluginsList::loadList()
{
	m_plugins.clear();

	QFile inputFile(fileName);
	if (inputFile.open(QIODevice::ReadOnly))
	{
		QTextStream in(&inputFile);
		while (!in.atEnd())
		{
			Plugin plugin;
			plugin.name = in.readLine();
			if (!plugin.name.isEmpty())
				m_plugins.push_back(plugin);
		}
		inputFile.close();
	}

	// Sort the list by name
	std::sort(m_plugins.begin(), m_plugins.end(), [](const Plugin& lhs, const Plugin& rhs){
		return lhs.name < rhs.name;
	});
}

void PluginsList::saveList()
{
	QFile outputFile(fileName);
	if(outputFile.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&outputFile);
		for(const Plugin& plugin : m_plugins)
			out << plugin.name << '\n';
	}
}

int PluginsList::find(const QString& name) const
{
	auto it = std::find_if(m_plugins.begin(), m_plugins.end(), [&name](const Plugin& plugin){
		return !plugin.name.compare(name, Qt::CaseInsensitive);
	});
	if(it != m_plugins.end())
		return it - m_plugins.begin();
	return -1;
}

void PluginsList::removePlugins(int first, int count)
{
	auto start = m_plugins.begin() + first;
	auto end = start + count;
	m_plugins.erase(start, end);
}

void PluginsList::addPlugin()
{
	Plugin plugin;
	plugin.name = "New Plugin";
	m_plugins.push_back(plugin);
}
