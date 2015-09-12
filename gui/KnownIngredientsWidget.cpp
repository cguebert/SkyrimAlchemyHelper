#include <QtWidgets>

#include "KnownIngredientsWidget.h"
#include "EffectsList.h"
#include "IngredientsList.h"
#include "GameSave.h"

KnownIngredientsModel::KnownIngredientsModel(GameSave& gamesave, QObject* parent)
	: QAbstractTableModel(parent)
	, m_ingredientsList(IngredientsList::instance())
	, m_effectsList(EffectsList::instance())
	, m_gameSave(gamesave)
{
	
}

int KnownIngredientsModel::rowCount(const QModelIndex& /*parent*/) const
{
	return m_gameSave.knownIngredients().size();
}

int KnownIngredientsModel::columnCount(const QModelIndex& /*parent*/) const
{
	return 5;
}

QVariant KnownIngredientsModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || role == Qt::EditRole)
	{
		const auto& ingredient = m_ingredientsList.ingredients()[index.row()];
		const auto& effects = m_effectsList.effects();
		switch (index.column())
		{
		case 0: return ingredient.name;
		case 1: return effects[ingredient.effects[0].effectId].name;
		case 2: return effects[ingredient.effects[1].effectId].name;
		case 3: return effects[ingredient.effects[2].effectId].name;
		case 4: return effects[ingredient.effects[3].effectId].name;
		}
		return QVariant();
	}
	else if (role == Qt::BackgroundRole)
	{
		if (index.column())
			return QVariant();

		const QColor unknownBackgroundColor = QColor(192, 192, 192);
		const auto& item = m_gameSave.knownIngredients()[index.row()];
		for (auto val : item)
			if (!val)
				return unknownBackgroundColor;
		return QVariant();
	}
	else if (role == Qt::ForegroundRole)
	{
		const QColor unknownTextColor = QColor(128, 128, 128);
		const auto& item = m_gameSave.knownIngredients()[index.row()];
		switch (index.column())
		{
		case 0: return QVariant();
		case 1: return item[0] ? QVariant() : unknownTextColor;
		case 2: return item[1] ? QVariant() : unknownTextColor;
		case 3: return item[2] ? QVariant() : unknownTextColor;
		case 4: return item[3] ? QVariant() : unknownTextColor;
		}
	}
	return QVariant();
}

QVariant KnownIngredientsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		switch (section)
		{
		case 0: return QString("ingredient");
		case 1: return QString("effect 1");
		case 2: return QString("effect 2");
		case 3: return QString("effect 3");
		case 4: return QString("effect 4");
		}
	}
	return QVariant();
}

void KnownIngredientsModel::beginReset()
{
	beginResetModel();
}

void KnownIngredientsModel::endReset()
{
	endResetModel();
}

//****************************************************************************//

KnownIngredientsWidget::KnownIngredientsWidget(GameSave& gamesave, QWidget* parent)
	: QWidget(parent)
{
	QVBoxLayout* vLayout = new QVBoxLayout;

	auto m_view = new QTableView(this);
	m_view->setSortingEnabled(true);
	m_model = new KnownIngredientsModel(gamesave, this);
	auto proxyModel = new QSortFilterProxyModel(this);
	proxyModel->setSourceModel(m_model);
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	m_view->setModel(proxyModel);
	m_view->sortByColumn(0, Qt::AscendingOrder);
	m_view->setSelectionMode(QAbstractItemView::NoSelection);
	m_view->horizontalHeader()->resizeSection(0, 250);
	vLayout->addWidget(m_view);

	setLayout(vLayout);
}

void KnownIngredientsWidget::beginReset()
{
	m_model->beginReset();
}

void KnownIngredientsWidget::endReset()
{
	m_model->endReset();
}
