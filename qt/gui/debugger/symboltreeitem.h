/******************************************************************************
 * treeitem.h - One entry in the Symbol tree
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
#ifndef TREEITEM_H
#define TREEITEM_H

#include <QVariant>
#include <QList>

//class NBTObject;
class QJsonObject;
//class NBTFile;

/** A classic Tree node.
 */
class SymbolTreeItem
{
public:
	explicit SymbolTreeItem(const QList<QVariant>& data, QJsonObject* obj, SymbolTreeItem* parentItem = nullptr);
	~SymbolTreeItem();

	void		appendChild(SymbolTreeItem* child);
	void		replaceChild(SymbolTreeItem* child, int row);
	SymbolTreeItem*	removeChild(int row);
	void		insertChild(SymbolTreeItem* child, int row);

	/// Get the path to this object. i.e. "map_0.dat:data:banners[1]:Pos"
	QString		getPath();

	SymbolTreeItem*	child(int row);
	int			childCount() const;
	int			columnCount() const;
	QVariant	data(int column) const;
	void		setDataValue(QVariant value, int column = 2);
	int			row() const;
	SymbolTreeItem*	parentItem();

	const QList<SymbolTreeItem*>&	getChildren();


	const QList<QVariant> getItemData();
	QString		getName();

private:
	QList<SymbolTreeItem*> m_childItems;
	QList<QVariant> m_itemData;
	SymbolTreeItem*		m_parentItem;
	QJsonObject*	m_jsonObject;	// pointer to original object.  We don't own this object.

	friend class TreeView;
};


inline const QList<QVariant> SymbolTreeItem::getItemData() { return(m_itemData); }
inline const QList<SymbolTreeItem*>&	SymbolTreeItem::getChildren() {return(m_childItems); }
inline QString SymbolTreeItem::getName() {
	QString s = m_itemData.at(0).toString();
	if (s.endsWith("*"))
		return(s.left(s.length()-1));
	else
		return(s);
}

#endif // TREEITEM_H
