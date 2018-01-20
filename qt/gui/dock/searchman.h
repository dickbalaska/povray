/******************************************************************************
 * searchman.h - The search manager i.e. "find in files"
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

#ifndef _SEARCHMAN_H_
#define _SEARCHMAN_H_

//#include <QObject>
#include <QStandardItem>
#include <QTreeView>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>

class MainWindow;
class SearchMan;
class SearchConsole;

enum SearchItemType
{
	SearchTypeFolder = QStandardItem::UserType,
	SearchTypeResult
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The SearchResult class
///
class SearchResult
{
public:
	QString		m_filepath;
	int			m_row;
	QString		m_lineText;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The SearchThread class
///
class SearchThread : public QThread
{
	Q_OBJECT

public:
	SearchThread(QObject* parent = NULL);
	~SearchThread();

	void			setParameters(const QString &searchString,
								  const QString& startDir,
								  const QStringList& filters);
	void			find();				// parent thread entry point
	SearchResult*	getResult();

signals:
	void	dataAvailableS();


protected:
	void	run() override;

private:
	QMutex				m_ctlMutex;		// lock for abort and restart
	QWaitCondition		m_ctlCondition;
	bool				m_restart;
	bool				m_abort;

	QMutex					m_dataMutex;	// lock for input and output queues
	QString					m_searchString;
	QString					m_startDir;
	QStringList				m_filters;
	QList<SearchResult*>	m_searchResults;

	QString				mT_searchString;
	QString				mT_startDir;
	QStringList			mT_filters;

	bool			processNode(const QString& node);
	void			sendMatch(const QString& node, int lineNumber, const QString& line);
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The SearchTreeItem class
///
class SearchTreeItem : public QStandardItem
{
	friend class SearchTreeView;
public:
	SearchTreeItem(SearchItemType type);
	void	decorate();

	virtual int type() {return(m_type); }

private:
	int		m_type;
	QString	m_node;
	int		m_lineNumber;
	QString	m_lineText;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The SearchTreeView class
///
class SearchTreeView : public QTreeView
{
	Q_OBJECT

	friend class SearchMan;
	friend class SearchConsole;

public:
	SearchTreeView(QWidget *parent);
	virtual ~SearchTreeView();
	void		insertResult(SearchResult* sr);
	void		moveToNext();
	void		moveToPrevious();

signals:
	void emitMoveToEditor(const QString& file, int line, int col);
	void emitMoveIsFromSearch();

protected:

	virtual void	mouseDoubleClickEvent(QMouseEvent* event);

private:
	QStandardItem*	findParent(QStandardItem* item, const QString& path);
	bool			walkModel(QModelIndex parent = QModelIndex());
	QString			rebuildPath(const SearchTreeItem* sti);

	QString				m_rootDir;
	QString				m_searchTerm;
	QStandardItemModel*	m_itemModel;
	Qt::ItemFlags		mFlags;			// constant flags added to each node
	QPixmap*			m_folderPm;
	QModelIndex			m_modelIndex;
	SearchTreeItem*		m_searchTreeItem;	// result of the recursive tree walker
	bool				m_takeNext;			// for the recursive tree walker
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The SearchMan class
///
class SearchMan : public QObject
{
	Q_OBJECT
public:
	explicit SearchMan(MainWindow* parent = NULL);

	void	setSearchConsole(SearchConsole* view);
signals:

public slots:
	void	findInFiles(const QString& text);
	void	dataAvailableR();
	void	moveToNext();
	void	moveToPrevious();


private:
	MainWindow*		m_mainWindow;
	SearchConsole*	m_searchConsole;
	QString			m_startDir;
	QStringList		m_filterList;
	bool			m_running;
	SearchThread	m_searchThread;
};

#endif // _SEARCHMAN_H_
