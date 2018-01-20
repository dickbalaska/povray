/******************************************************************************
 * searchman.cpp - The search manager i.e. "find in files"
 *
 * qtpov - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
 *
 * qtpov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <QHeaderView>
#include <QLineEdit>
#include <QDebug>

#include "dockman.h"
#include "mainwindow.h"
#include "findman.h"
#include "workspace.h"
#include "searchman.h"

static bool SDEBUG = false;

SearchTreeItem::SearchTreeItem(SearchItemType type)
{
	m_type = type;
}

void SearchTreeItem::decorate()
{
	QString s;
	s = m_node + ":" + QString::number(m_lineNumber) + " - " + m_lineText;
	this->setText(s);
}
///////////////////////////////////////////////////////////////////////////////
SearchTreeView::SearchTreeView(QWidget* parent)
	: QTreeView(parent)
{
	m_folderPm = new QPixmap(":/resources/icons/folder.png");
	//header()->hide();
	//this->set
	mFlags = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
	m_itemModel = new QStandardItemModel();
	m_itemModel->setColumnCount(1);
	m_itemModel->setHorizontalHeaderLabels(QStringList(""));
	this->setModel(m_itemModel);
}

SearchTreeView::~SearchTreeView()
{
	delete m_itemModel;
	delete m_folderPm;
}
void SearchTreeView::insertResult(SearchResult* sr)
{
	QString path = sr->m_filepath.mid(m_rootDir.length());
	if (path.startsWith('/'))
		path = path.mid(1);
	if (SDEBUG) qDebug() << "insertResult path" << path;
	QStandardItem* parent = findParent(m_itemModel->invisibleRootItem(), path);
	SearchTreeItem* item = new SearchTreeItem(SearchTypeResult);
	int i = path.lastIndexOf('/');
	QString node;
	if (i != -1)
		node = path.mid(i+1);
	else
		node = path;
	item->setFlags(mFlags);
	item->m_node = node;
	item->m_lineNumber = sr->m_row;
	item->m_lineText = sr->m_lineText;
	item->decorate();
	parent->appendRow(item);
}

QStandardItem* SearchTreeView::findParent(QStandardItem* item, const QString& path)
{
	QString node;
	QString remainder;
	int i;
	if ((i = path.indexOf('/')) != -1) {
		node = path.left(i);
		remainder = path.mid(i+1);
	}
	if (node.isEmpty())
		return(item);
	if (SDEBUG) qDebug() << "findParent: checkNode" << node << "remainder" << remainder;
	SearchTreeItem* sti;
	for (i=0; ; i++) {
		sti = (SearchTreeItem*)item->child(i);
		if (sti != NULL) {
			if (sti->m_node == node) {
				if (remainder.contains('/'))
					return(findParent(sti, remainder));
				return(sti);
			}
		} else
			break;
	}
	sti = new SearchTreeItem(SearchTypeFolder);
	sti->setFlags(mFlags);
	sti->setText(node);
	sti->m_node = node;
	sti->setIcon(QIcon(*m_folderPm));
	item->appendRow(sti);
	if (remainder.contains('/'))
		return(findParent(sti, remainder));
	return(sti);
}

void SearchTreeView::mouseDoubleClickEvent(QMouseEvent* event)
{
	m_modelIndex = indexAt(event->pos());
	//QStandardItemModel* qsim = (QStandardItemModel*)this->model();
	//QModelIndex qmi = qsim->currentIndex();
	SearchTreeItem* sti = (SearchTreeItem*)m_itemModel->itemFromIndex(m_modelIndex);
	QString path = rebuildPath(sti);
	int col = sti->m_lineText.indexOf(m_searchTerm);
	if (col == -1) {
		qWarning() << "failed to find search term in matched line?" << m_searchTerm;
		col = 0;
	}
	if (sti->m_type == SearchTypeFolder)
		return;
	qDebug() << "emit" << path;
	emit(emitMoveToEditor(path, sti->m_lineNumber, col+1));
	emit(emitMoveIsFromSearch());
}

QString SearchTreeView::rebuildPath(const SearchTreeItem* sti)
{
	QString path = sti->m_node;
	while (sti->parent() != NULL) {
		sti = (SearchTreeItem*)sti->parent();
		path = sti->m_node + "/" + path;
	}
	path = m_rootDir + "/" + path;
	return(path);
}

bool SearchTreeView::walkModel(QModelIndex parent) {
	for(int r = 0; r < m_itemModel->rowCount(parent); ++r) {
		QModelIndex index = m_itemModel->index(r, 0, parent);
		SearchTreeItem* sti  = (SearchTreeItem*)m_itemModel->itemFromIndex(index);
		if (sti->m_type == SearchTypeResult) {
			if (m_takeNext) {
				m_searchTreeItem = sti;
				m_modelIndex = index;
				return(true);
			}
			if (index == m_modelIndex) {
				m_takeNext = true;
			}
		}
		if( m_itemModel->hasChildren(index) ) {
			if (walkModel(index))
				return(true);
		}
	}
	return(false);
}

void SearchTreeView::moveToNext()
{
	m_takeNext = false;
	m_searchTreeItem = NULL;
	if (!m_modelIndex.isValid())
		m_takeNext = true;
	walkModel();
	if (!m_searchTreeItem)
		return;
//	QModelIndex mip = m_modelIndex.parent();
//	expand(mip);
//	setExpanded(m_modelIndex, true);
	this->setCurrentIndex(m_modelIndex);
	QString path = rebuildPath(m_searchTreeItem);
	int col = m_searchTreeItem->m_lineText.indexOf(m_searchTerm);
	qDebug() << "emit" << path;
	emit(emitMoveToEditor(path, m_searchTreeItem->m_lineNumber, col+1));

}


void SearchTreeView::moveToPrevious()
{

}
///////////////////////////////////////////////////////////////////////////////
SearchMan::SearchMan(MainWindow* parent)
	: QObject(NULL),
	  m_mainWindow(parent),
	  //m_searchView(searchView),
	  m_running(false)
{
	connect(&m_searchThread, SIGNAL(dataAvailableS()), this, SLOT(dataAvailableR()));

}

void	SearchMan::setSearchConsole(SearchConsole* console)
{
	m_searchConsole = console;
	connect(m_searchConsole, SIGNAL(emitMoveToEditor(QString,int,int)), m_mainWindow, SLOT(moveToEditor(QString,int,int)));
	connect(m_searchConsole, SIGNAL(emitMoveIsFromSearch()), m_mainWindow->getFindMan(), SLOT(moveIsFromSearch()));
}

void	SearchMan::findInFiles(const QString& text)
{
	// XXX Need to handle the "already running" condition.
	m_mainWindow->getDockMan()->getConsoleDock()->showConsole(1);
	m_searchConsole->setSearchTerm(text);
	m_searchConsole->m_rootDir =  m_mainWindow->getWorkspace()->getDirRoot();
	//m_filterList = m_mainWindow->getWorkspace()->getResourceFilters();
	m_searchThread.setParameters(text,
								 m_mainWindow->getWorkspace()->getDirRoot(),
								 m_mainWindow->getWorkspace()->getResourceFilters());
	m_searchThread.find();
}

void SearchMan::dataAvailableR()
{
	SearchResult* sr;
	while ((sr = m_searchThread.getResult()) != NULL) {
		if (SDEBUG) qDebug() << "Search Result:" << sr->m_filepath << sr->m_row << sr->m_lineText;
		m_searchConsole->insertResult(sr);
		delete sr;
	}
}
void SearchMan::moveToNext() { m_searchConsole->moveToNext(); }
void SearchMan::moveToPrevious() { m_searchConsole->moveToPrevious(); }

///////////////////////////////////////////////////////////////////////////////
SearchThread::SearchThread(QObject* parent)
	: QThread(parent)
{
	m_restart = false;
	m_abort = false;
}

SearchThread::~SearchThread()
{
	m_ctlMutex.lock();
	m_abort = true;
	m_ctlCondition.wakeOne();
	m_ctlMutex.unlock();
	wait();
}

void SearchThread::setParameters(const QString& searchString, const QString& startDir, const QStringList& filters)
{
	QMutexLocker locker(&m_dataMutex);
	m_searchString = searchString;
	m_startDir = startDir;
	m_filters = filters;
//	foreach (QString s, filters) {
//		m_filters.append(QRegExp(s, Qt::CaseInsensitive, QRegExp::Wildcard));
//	}
}

void SearchThread::find()
{
	if (!isRunning()) {
		start();
	} else {
		m_restart = true;
		m_ctlCondition.wakeOne();
	}
}

SearchResult*	SearchThread::getResult()
{
	QMutexLocker locker(&m_dataMutex);
	if (m_searchResults.isEmpty())
		return(NULL);
	SearchResult* sr = m_searchResults.first();
	m_searchResults.removeFirst();
	return(sr);
}

void SearchThread::run()
{
	forever {
		m_dataMutex.lock();
		mT_searchString = m_searchString;
		mT_startDir = m_startDir;
		mT_filters = m_filters;
		while (!m_searchResults.isEmpty()) {
			SearchResult* sr = m_searchResults.first();
			m_searchResults.removeFirst();
			delete sr;
		}
		m_dataMutex.unlock();
#ifdef _DEBUG
		qDebug() << "SearchThread::run" << mT_startDir;
		qDebug() << "mT_filters" << mT_filters;
#endif

		processNode(m_startDir);

		if (m_abort)
			return;

		// park after end of run
		m_ctlMutex.lock();
		if (!m_restart)
			m_ctlCondition.wait(&m_ctlMutex);
		m_restart = false;
		m_ctlMutex.unlock();
		if (m_abort)
			return;
	}
}

bool SearchThread::processNode(const QString& node)
{
	QFileInfo	qfi(node);
	if (qfi.isDir()) {
		QDir qdi(node);
		//qdi.setNameFilters(mT_filters);
		QFileInfoList qfil = qdi.entryInfoList(mT_filters,
											   QDir::AllDirs|QDir::NoDotAndDotDot|QDir::Files,
											   QDir::DirsFirst);
		foreach(QFileInfo qfi2, qfil) {
			if (!processNode(qfi2.absoluteFilePath()))
				return(false);
		}
		return(true);
	}
	if (SDEBUG) qDebug() << "grep node:" << node;
	QFile file(node);
	if (!file.open(QIODevice::ReadOnly)) {
		qWarning() << "SearchThread: failed to open file:" << node;
		return(true);
	}
	QTextStream in(&file);
	QString line;
	int lineNumber = 0;
	do {
		lineNumber++;
		line = in.readLine();
		if (line.contains(mT_searchString))
			sendMatch(node, lineNumber, line);
	} while (!line.isNull());

	return(true);
}

void SearchThread::sendMatch(const QString &node, int lineNumber, const QString &line)
{
	SearchResult*	sr = new SearchResult;
	sr->m_filepath	= node;
	sr->m_row		= lineNumber;
	sr->m_lineText	= line;
	if (SDEBUG) qDebug() << "T:sendMatch" << node << lineNumber << line;
	QMutexLocker locker(&m_dataMutex);
	m_searchResults.append(sr);
	emit(dataAvailableS());
}
