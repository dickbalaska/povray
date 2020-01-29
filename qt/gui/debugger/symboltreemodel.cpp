/******************************************************************************
 * treemodel.cpp - Extend QAbstractItemModel to support our tree
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
#include <QJsonObject>
#include <QDebug>

#include "../source/parser/povdbgobjectnames.h"

#include "symboltreeitem.h"
#include "symboltreemodel.h"

///////////////////////////////////////////////////////////////////////////////
SymbolTreeModel::SymbolTreeModel(QObject* parent)
	: QAbstractItemModel(parent),
	  m_parent(parent)
{
	QList<QVariant> rootData;
	rootData << "Name" << "Type" << "Value";
	m_rootItem = new SymbolTreeItem(rootData, nullptr);
	connect(this, SIGNAL(dataEdited(const QModelIndex&, const QVariant)), parent, SLOT(onDataEdited(const QModelIndex&, const QVariant)));
}

SymbolTreeModel::~SymbolTreeModel()
{
	delete m_rootItem;
}

QModelIndex SymbolTreeModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	SymbolTreeItem* parentItem;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<SymbolTreeItem*>(parent.internalPointer());

	SymbolTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex SymbolTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	SymbolTreeItem *childItem = static_cast<SymbolTreeItem*>(index.internalPointer());
	SymbolTreeItem *parentItem = childItem->parentItem();

	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int SymbolTreeModel::rowCount(const QModelIndex &parent) const
{
	SymbolTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<SymbolTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

int SymbolTreeModel::columnCount(const QModelIndex& ) const
{
//	if (parent.isValid())
//		return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
//	else
		return m_rootItem->columnCount();
}

QVariant SymbolTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	SymbolTreeItem* item = static_cast<SymbolTreeItem*>(index.internalPointer());
	if (role == Qt::EditRole) {
		return item->data(index.column());

	}
	if (role != Qt::DisplayRole)
		return QVariant();


	return item->data(index.column());
}

bool SymbolTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return(false);
	if (role == Qt::EditRole) {
		emit (dataEdited(index, value));
		return(false);
	}
	if (role != Qt::DisplayRole)
		return(false);
	SymbolTreeItem* item = static_cast<SymbolTreeItem*>(index.internalPointer());
	item->setDataValue(value, index.column());
	emit (dataChanged(index, index));
	return(true);
}

SymbolTreeItem* SymbolTreeModel::getTreeItem(const QModelIndex& index)
{
	SymbolTreeItem* item = static_cast<SymbolTreeItem*>(index.internalPointer());
	return(item);
}

// XXX: temp is readonly (?)
Qt::ItemFlags SymbolTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return nullptr;
	Qt::ItemFlags qif = QAbstractItemModel::flags(index);
	if (index.column() == tmType)	// can't edit the type
		return(qif);
	qif |= Qt::ItemIsEditable;
	return qif;
}

QVariant SymbolTreeModel::headerData(int section, Qt::Orientation orientation,
							   int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_rootItem->data(section);

	return QVariant();
}

void SymbolTreeModel::replaceRow(QModelIndex& parent, int row, SymbolTreeItem* treeItem)
{
	beginInsertRows(parent, row, row);
	SymbolTreeItem* item = static_cast<SymbolTreeItem*>(parent.internalPointer());
	if (!item)
		item = m_rootItem;
	item->replaceChild(treeItem, row);
	endInsertRows();
}

SymbolTreeItem* SymbolTreeModel::removeRow(QModelIndex& parent, int row)
{
	//QModelIndex qmi = index(row, 0, parent);
	beginRemoveRows(parent, row, row);
	SymbolTreeItem* item = static_cast<SymbolTreeItem*>(parent.internalPointer());
	SymbolTreeItem* child = item->removeChild(row);
	endRemoveRows();
//	NBTObject* pobj = item->getObject();
//	if (pobj && pobj->type == TAG_List) {
//		//pobj->d_list->objcount--;
//		int newCount = pobj->d_list->objcount;
////		QModelIndex columnSibling = parent.siblingAtColumn(2);
//		QModelIndex columnSibling = parent.sibling(parent.row(), 2);
//		QMap<int, QVariant> data;
//		data[Qt::DisplayRole] = QVariant(newCount);
//		bool res = this->setItemData(columnSibling, data);
//		qDebug() << "removeRow.setItemData returned" << res;
//	}

	return(child);
}

void SymbolTreeModel::insertRow(QModelIndex &parent, int row, SymbolTreeItem* treeItem)
{
	beginInsertRows(parent, row, row);
	SymbolTreeItem* tip = static_cast<SymbolTreeItem*>(parent.internalPointer());
	tip->insertChild(treeItem, row);
	endInsertRows();
}

void SymbolTreeModel::addWatch(const QJsonObject& obj)
{
//	if (root && root->type != TAG_Compound) {
//		qCritical() << "TreeModel expected root to be a compound";
//		return(nullptr);
//	}
	QList<QVariant> columnData;
	QString symName = obj[s_name].toString();
	SymbolTreeItem* sti = buildTreeNode(m_rootItem, obj, true);
	for (int i=0; i<this->rowCount(); i++) {
		QModelIndex qmi = this->index(i, 0);
		QVariant data = this->data(qmi);
		QString s = data.toString();
		if (symName == s) {
			QModelIndex parent;
			qDebug() << "SymbolTreeModel::addWatch" << symName;
			this->replaceRow(parent, i, sti);
			return;
		} 
	}
	qDebug() << "SymbolTreeModel::addWatch" << symName;
	m_rootItem->appendChild(sti);
//	if (!root) {
//		QString s = QString("--%1").arg(label);
//		columnData << QVariant(s);
//	} else {
//		columnData << QVariant(label);
//	}
//	SymbolTreeItem* ti = new SymbolTreeItem(columnData, root, m_rootItem);
//	if (!noInsert)
//		m_rootItem->appendChild(ti);
//	if (!root)
//		return(ti);
////	foreach(NBTObject* nobj, *root->d_compound) {
////		setupTree(nobj, ti);
////	}
//	return(ti);
}


SymbolTreeItem* SymbolTreeModel::buildTreeNode(SymbolTreeItem* parent, const QJsonObject& obj, bool isRoot)
{
	QList<QVariant> columnData;
	QString symName = obj[s_name].toString();
	QString typeS = obj[s_typeS].toString();
	QString value = obj[s_value].toString();
	columnData << symName;
	columnData << typeS;
	columnData << value;
	SymbolTreeItem* sti = new SymbolTreeItem(columnData, parent);
	if (!isRoot && parent)
		parent->appendChild(sti);
	return(sti);
//	columnData << obj->name;
//	columnData << nbtTypeNames[obj->type];
//	switch (obj->type) {
//	case TAG_Byte:
//		columnData << QString("%1").arg(obj->d_byte);
//		break;
//	case TAG_Short:
//		columnData << QString("%1").arg(obj->d_short);
//		break;
//	case TAG_Int:
//		columnData << QString("%1").arg(obj->d_int);
//		break;
//	case TAG_Long:
//		columnData << QString("%1").arg(obj->d_long);
//		break;
//	case TAG_Float:
//		columnData << QString("%1").arg(obj->d_float);
//		break;
//	case TAG_Double:
//		columnData << QString("%1").arg(obj->d_double);
//		break;
//	case TAG_Byte_Array:
//		columnData << QString("%1").arg(obj->d_byte_array->size());
//		break;
//	case TAG_Int_Array:
//		columnData << QString("%1").arg(obj->size);
//		break;
//	case TAG_Long_Array:
//		columnData << QString("%1").arg(obj->size);
//		break;
//	case TAG_String:
//		columnData << *obj->d_string;
//		break;
//	case TAG_List: {
//		NList* nlist = obj->d_list;
//		columnData << nlist->objcount;
//		break;
//	}
//	}
//	TreeItem* ti = new TreeItem(columnData, obj, parent);
//	parent->appendChild(ti);
//	if (obj->type == TAG_Compound) {
//		if (obj->d_compound) {
//			foreach(NBTObject* nobj, *obj->d_compound) {
//				setupTree(nobj, ti);
//			}
//		}
//	}
//	if (obj->type == TAG_List) {
//		NList* nlist = obj->d_list;
//		if (nlist->objtype == TAG_Compound) {
//			for (int i=0; i<nlist->objcount; i++) {
//				setupTree(nlist->d_compound->at(i), ti);
//			}
//		} else {
//			for (int i=0; i<nlist->objcount; i++) {
//				QList<QVariant> cdata;
//				cdata << QString("");
//				cdata << nbtTypeNames[nlist->objtype];
//				switch (nlist->objtype) {
//				case TAG_String:
//					cdata << nlist->d_string->at(i);
//					break;
//				case TAG_Byte:
//					cdata << QString("%1").arg(nlist->d_byte[i]);
//					break;
//				case TAG_Short:
//					cdata << QString("%1").arg(nlist->d_short[i]);
//					break;
//				case TAG_Int:
//					cdata << QString("%1").arg(nlist->d_int[i]);
//					break;
//				case TAG_Long:
//					cdata << QString("%1").arg(nlist->d_long[i]);
//					break;
//				case TAG_Float:
//					cdata << QString("%1").arg(nlist->d_float[i]);
//					break;
//				case TAG_Double:
//					cdata << QString("%1").arg(nlist->d_double[i]);
//					break;

//				}
//				TreeItem* lti = new TreeItem(cdata, obj, ti);
//				ti->appendChild(lti);
//			}
//		}
//	}
}

SymbolTreeItem* SymbolTreeModel::getTreeItem(const QString& path, SymbolTreeItem* node)
{
	QStringList segments = path.split(":");
	int	index;
	QString	segName = segments[0];
	if ((index = segName.indexOf('[')) != -1) {
		QString s = segName.mid(index);	// capture between the []
		s = s.left(s.length()-1);
		segName = segName.left(index);
		index = s.toInt();
	}
	QString nodeName = node->getName();
	if (nodeName == segName) {
		segments.takeFirst();
		if (index != -1)
			node = node->child(index);
		if (segments.isEmpty())
			return(node);
		QString cname = segments.join(":");
		for (SymbolTreeItem* ti : node->getChildren()) {
			SymbolTreeItem* res = getTreeItem(cname, ti);
			if (res)
				return(res);
		}
		return(nullptr);
	}
	if (segName == "*") {
		segments.takeFirst();
		QString cname = segments.join(":");
		for (SymbolTreeItem* ti : node->getChildren()) {
			SymbolTreeItem* res = getTreeItem(cname, ti);
			if (res)
				return(res);
		}
	}
	return(nullptr);
}

QModelIndex SymbolTreeModel::getIndex(const QString& path)
{
	QStringList segments = path.split(":");
	QModelIndex qmi;
	SymbolTreeItem* ti = m_rootItem;
	qmi = index(0,0);
	while (!segments.isEmpty()) {
		int li;
		QString segName = segments[0];
		if ((li = segName.indexOf('[')) != -1) {
			QString s = segName.mid(li);	// capture between the []
			s = s.left(s.length()-1);
			segName = segName.left(li);
			li = s.toInt();
		}
		bool found = false;
		int row = 0;
		for (SymbolTreeItem* tti : ti->getChildren()) {
			if (tti->getName() == segName) {
				qmi = index(row, 0, qmi);
				ti = ti->child(row);
				found = true;
				segments.takeFirst();
				break;
			}
			row++;
		}
		if (!found) {
			qWarning() << "TreeModel:getIndex failed" << path;
			return(QModelIndex());
		}
		if (li != -1) {
			qmi = index(li, 0, qmi);
			ti = ti->child(li);
		}
	}
	return(qmi);
}

bool SymbolTreeModel::isEmpty()
{
	return(m_rootItem->childCount() == 0);
}
