/******************************************************************************
 * treeview.cpp - The watch Symbol table TreeView
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
#include <QMessageBox>
#include <QKeyEvent>
#include <QDebug>

#include "symboltreeitem.h"
#include "symboltreeview.h"
#include "mainwindow.h"
#include "dock/resourcedock.h"
//#include "commandman.h"
//#include "commanddelete.h"

static const int SlcMax = 3;

///////////////////////////////////////////////////////////////////////////////
SymbolTreeView::SymbolTreeView(QWidget* parent)
	: QTreeView(parent)

{
	//m_mainWindow = m_resourceDock->m_mainWindow;
}

// Set from user's preference or default
void SymbolTreeView::setupColumnWidths()
{
//	int	slcs[SlcMax];
//	m_resourceDock->m_mainWindow->getResourceColumnWidths(slcs, SlcMax);
//	bool resizeOk = true;
//	for (int i=0; i<SlcMax; i++)
//		if (slcs[i] <= 0)
//			resizeOk = false;
//	if (resizeOk) {
//		for (int i=0; i<SlcMax; i++)
//			this->setColumnWidth(i, slcs[i]);
//	}
//	else {
		for (int i=0; i<SlcMax; i++)
			this->resizeColumnToContents(i);
//	}

}
void SymbolTreeView::updateColumnSizeHints()
{
	int	slcs[SlcMax];
	for (int i=0; i<SlcMax; i++)
		slcs[i] = this->columnWidth(i);
//	m_resourceDock->m_mainWindow->setResourceColumnWidths(slcs, SlcMax);
}

void SymbolTreeView::keyPressEvent(QKeyEvent *event)
{
//	if (event->key() == Qt::Key_Delete) {
//		//qDebug() << "Delete!";
//		event->accept();
//		QModelIndexList qmil = selectedIndexes();
//		if (qmil.isEmpty())
//			return;
//		QModelIndex qmi = qmil.first();
//		TreeItem* item = static_cast<TreeItem*>(qmi.internalPointer());
//		if (!item->parentItem() || !item->parentItem()->parentItem()) {
//			QString s = QString("Won't delete entire file %1").arg(item->m_itemData.at(0).toString());
//			QMessageBox::warning(this, "Warning", s, QMessageBox::NoButton);
//		}
//		qDebug() << "row=" << qmi.row();
//		CommandDelete* cd = new CommandDelete();
//		cd ->setGroup(m_resourceDock->getCurrentGroup());
//		cd->setModelIndex(qmi);
//		m_mainWindow->getCommandMan()->doCommand(cd);
//		return;
//	}
	super::keyPressEvent(event);
}

