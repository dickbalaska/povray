/******************************************************************************
 * treemodel.h - Extend QAbstractItemModel to support our tree
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2020 - Dick Balaska, and BuckoSoft.
 *
 * qtpovray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpovray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>

//class World;
class SymbolTreeItem;
//class NBTObject;
//class RegionFile;

enum {
	tmName,
	tmType,
	tmData
};

class SymbolTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit SymbolTreeModel(QObject* parent);
	virtual ~SymbolTreeModel() override;

	QVariant		data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	bool			setData(const QModelIndex &index, const QVariant &value, int role) override;
	Qt::ItemFlags	flags(const QModelIndex& index) const override;
	QVariant		headerData(int section, Qt::Orientation orientation,
							   int role = Qt::DisplayRole) const override;
	QModelIndex		index(int row, int column,
						  const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex		parent(const QModelIndex& index) const override;
	int				rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int				columnCount(const QModelIndex& parent = QModelIndex()) const override;

	bool		isEmpty();
	void		addWatch(const QJsonObject& obj);
	void		removeWatch(const QString& name);

	SymbolTreeItem*	getTreeItem(const QModelIndex& index);
	SymbolTreeItem*	getTreeItem(const QString& path, SymbolTreeItem* node);
	QModelIndex	getIndex(const QString& path);
	SymbolTreeItem*	getRootNode();

	void		replaceRow(QModelIndex& parent, int row, SymbolTreeItem* treeItem);
	void		removeRow(QModelIndex& parent, int row);
	void		insertRow(QModelIndex& parent, int row, SymbolTreeItem* treeItem);

signals:
	void		dataEdited(const QModelIndex& index, const QVariant data);

private:
	void setupModelData(QJsonObject* root, SymbolTreeItem* parent);
	void setupTree(QJsonObject* obj, SymbolTreeItem* parent);
	SymbolTreeItem* buildTreeNode(SymbolTreeItem* parent, const QJsonObject& obj, bool isRoot);

	SymbolTreeItem*	m_rootItem;
	QObject*	m_parent;
};

inline SymbolTreeItem* SymbolTreeModel::getRootNode() { return(m_rootItem); }

#endif // TREEMODEL_H
