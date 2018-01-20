/******************************************************************************
 * maintoolbar.h - The primary toolbar for qtpov
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

#ifndef _MAINTOOLBAR_H_
#define _MAINTOOLBAR_H_

#include <QToolBar>
#include <QMenu>
#include "dropdown.h"
#include "mainwindow.h"

class MainToolbar : public QToolBar
{
	Q_OBJECT

public:
	explicit MainToolbar(const QString &title, bool useLargeIcons, MainWindow* parent);
	virtual ~MainToolbar();

	void	enableSave(bool enable);
	void	enableInsertMenu(bool enable);
	void	enableRender(bool enable);
	void	renderButtonToStart();
	void	renderButtonToStop();
	void	updateMRU(const QStringList& mru);
	void	updateUndoRedo(CodeEditor* ce);
	void	tabChanged(int index);				// user selected a different tab/editor. Update menu states.

	bool	isRenderButtonStart() { return(renderButtonIsStart); }
	Dropdown*	getRenderCL() { return(renderCl); }
	QMenu*	getInsertMenu() { return(insertMenu); }

public slots:
	void	onCopyAvailable(bool yes);

private slots:
	void	doCut();
	void	doCopy();
	void	doPaste();
	void	doUndo();
	void	doRedo();
	void	doGotoMatchingBrace();
	void	doGotoLineNumber();

private:
	void setupMenu();

	QMenu*		insertMenu;
	QAction*	openAction;
	QAction*	saveAction;
	QAction*	saveAllAction;
	QAction*	renderAction;
	QAction*	m_editUndoAction;
	QAction*	m_editRedoAction;
	QAction*	m_editCutAction;
	QAction*	m_editCopyAction;
	QAction*	m_editPasteAction;
	QAction*	m_editSelectAllAction;
	QAction*	m_editAdvIndentAction;
	QAction*	m_editAdvUnindentAction;
	QAction*	m_editAdvToggleComments;
	QAction*	m_editGotoMatchingBrace;
	QAction*	m_editGotoLineNumber;
	QAction*	m_editGotoNextBookmark;
	QAction*	m_editGotoPrevBookmark;
	QAction*	m_editPreferencesAction;

	Dropdown*	renderCl;

	MainWindow*	m_mainWindow;
	bool		renderButtonIsStart;

	QAction*		recentFileActions[MaxRecentFiles];
	QMetaObject::Connection recentFileConnections[MaxRecentFiles];

};

#endif // _MAINTOOLBAR_H_
