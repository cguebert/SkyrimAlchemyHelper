#include <QtWidgets>

#include "EffectsListWidget.h"
#include "Config.h"

EffectsListModel::EffectsListModel(Config& config, QObject* parent)
	: QAbstractTableModel(parent)
	, m_config(config)
{
}

int EffectsListModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_config.effects.size();
}

int EffectsListModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 4;
}

QVariant EffectsListModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const auto& effect = m_config.effects[index.row()];
		switch (index.column())
		{
		case 0: return effect.name;
		case 1: return QString::number(effect.code, 16).toUpper();
		case 2: return effect.baseCost;
		case 3: return effect.ingredients.size();
		}
		return QVariant();
	}
	return QVariant();
}

QVariant EffectsListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0: return QString("name");
		case 1: return QString("id");
		case 2: return QString("base cost");
		case 3: return QString("# ingredients");
		}
	}
	return QVariant();
}

void EffectsListModel::beginReset()
{
	beginResetModel();
}

void EffectsListModel::endReset()
{
	endResetModel();
}

//****************************************************************************//

EffectsListWidget::EffectsListWidget(Config& config, QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new EffectsListModel(config, this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_view->setModel(proxyModel);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->horizontalHeader()->resizeSection(0, 250);
	m_view->setSelectionMode(QAbstractItemView::NoSelection);
	m_view->hideColumn(1);
	vLayout->addWidget(m_view);

	setLayout(vLayout);
}

void EffectsListWidget::beginReset()
{
	m_model->beginReset();
}

void EffectsListWidget::endReset()
{
	m_model->endReset();
}
