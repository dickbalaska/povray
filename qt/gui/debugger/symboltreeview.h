/******************************************************************************
 * treeview.h - The TreeView in the ResourceDock
 *
 * yamce - Yet Another Minecraft Editor
 * Copyright(c) 2019 - Dick Balaska, and BuckoSoft.
 *
 * yamce is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * yamce is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#ifndef SYMBOLTREEVIEW_H
#define SYMBOLTREEVIEW_H

#include <QTreeView>

class ResourceDock;
class MainWindow;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief The primary object in the ResourceDock
class SymbolTreeView : public QTreeView
{
private:
	typedef QTreeView super;
	Q_OBJECT

public:
	explicit SymbolTreeView(QWidget* parent = nullptr);

	void		updateColumnSizeHints();
	void		setupColumnWidths();

signals:
	void	removeWatch(const QString& name);
	
protected:
	virtual void keyPressEvent(QKeyEvent *event);

private:
//	ResourceDock*	m_resourceDock;
	MainWindow*		m_mainWindow;
	//TreeModel*		m_treeModel;

};


#endif // SYMBOLTREEVIEW_H
