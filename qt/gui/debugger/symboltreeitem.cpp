/******************************************************************************
 * treeitem.cpp - One entry in the Symbol tree
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
#include "symboltreeitem.h"

SymbolTreeItem::SymbolTreeItem(const QList<QVariant>& data, QJsonObject* obj, SymbolTreeItem* parent)
{
	m_itemData = data;
	m_jsonObject = obj;
	m_parentItem = parent;
}

SymbolTreeItem::~SymbolTreeItem()
{
	qDeleteAll(m_childItems);
}

void SymbolTreeItem::appendChild(SymbolTreeItem* item)
{
	m_childItems.append(item);
}

void SymbolTreeItem::replaceChild(SymbolTreeItem* child, int row)
{
	SymbolTreeItem* old = m_childItems[row];
	m_childItems[row] = child;
	delete old;
}

SymbolTreeItem* SymbolTreeItem::removeChild(int row)
{
	SymbolTreeItem* child = m_childItems[row];
	m_childItems.removeAt(row);
	return(child);
}

void SymbolTreeItem::insertChild(SymbolTreeItem *child, int row)
{
	m_childItems.insert(row, child);
}

SymbolTreeItem* SymbolTreeItem::child(int row)
{
	return m_childItems.value(row);
}

int SymbolTreeItem::childCount() const
{
	return m_childItems.count();
}

int SymbolTreeItem::row() const
{
	if (m_parentItem)
		return m_parentItem->m_childItems.indexOf(const_cast<SymbolTreeItem*>(this));
	return 0;
}

int SymbolTreeItem::columnCount() const
{
	return m_itemData.count();
}

QVariant SymbolTreeItem::data(int column) const
{
	return m_itemData.value(column);
}

void SymbolTreeItem::setDataValue(QVariant value, int column)
{
	m_itemData[column] = value;
}

SymbolTreeItem* SymbolTreeItem::parentItem()
{
	return m_parentItem;
}

QString	SymbolTreeItem::getPath()
{
	QString	output;
	SymbolTreeItem* parent = this->parentItem();
//	NBTObject* obj;
	SymbolTreeItem*	ti = this;
	QString s;
	while (parent) {
//		if (!parent->parentItem())
//			break;			// Don't include the hidden root in our path
//		obj = ti->getObject();
//		if (!obj)
//			break;
		s = ti->m_itemData.at(0).toString();
		if (!output.isEmpty())
			output = QString("%1:%2").arg(s, output);
		else {
			output = s;
		}
		ti = parent;
		parent = parent->parentItem();
	}
	return(output);
}

