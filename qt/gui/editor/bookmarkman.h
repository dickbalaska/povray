/******************************************************************************
 * bookmarkman.h - The Bookmark Manager
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

#ifndef _BOOKMARKMAN_H_
#define _BOOKMARKMAN_H_

#include <QObject>

class MainWindow;
class CodeEditor;
class Workspace;

enum BookmarkCommand {
	bmToggle,
	bmNext,
	bmPrevious
};

class Bookmark : public QObject
{
public:
	QString	m_pathName;
	int		m_lineNumber;
};

class BookmarkMan : public QObject
{
	Q_OBJECT

	friend class Workspace;

public:
	explicit BookmarkMan(MainWindow* parent);
	virtual ~BookmarkMan();

	void		addBookmark(Bookmark* bm);
	QList<int>	gatherBookmarks(CodeEditor* ce);

signals:
	void emitMoveToEditor(const QString& file, int line, int col);

public slots:
	void	onBookmarkCommand(int bm, int lineNumber = 0);
	void	onBookmarkToggle(int lineNumber = 0);
	void	onBookmarkNext(int lineNumber = 0);
	void	onBookmarkPrevious(int lineNumber = 0);
	void	onUpdateBookmarks(const QList<int>& list);

private:

	MainWindow*			m_mainWindow;

	QList<Bookmark*>	m_bookmarks;
};

#endif // _BOOKMARKMAN_H_
