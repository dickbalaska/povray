/******************************************************************************
 * mainwindow.h - The Qt QMainWindow for qtpov
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
#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include <QMainWindow>
#include <QCloseEvent>
#include <QShortcut>
#include <QDialog>

#ifdef QT_QML_DEBUG		// This sucks, but I can't find another way to define _DEBUG for only debug
#define	_DEBUG	1
#endif

#include "preferencedata.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLineEdit;
class QCheckBox;
class QRadioButton;
QT_END_NAMESPACE

class BookmarkMan;
class CodeEditor;
class DockMan;
class FindMan;
class Highlighter;
class InsertMenuMan;
class MainToolbar;
class SearchMan;
class StatusBar;
class VfeClient;
class Workspace;

#ifdef USE_WEBSOCKETS
class WsClient;
#endif
enum
{
	BANNER_STREAM = 0,
	DEBUG_STREAM,
	INFORMATION_STREAM,
	FATAL_STREAM,
	RENDER_STREAM,
	STATISTIC_STREAM,
	WARNING_STREAM,
	ERROR_STREAM,		// qtpov specific. Parse the error message data
	QTINFO_STREAM,		// qtpov specific. Display the render command
	ALL_STREAM,
	MAX_STREAMS
};

enum FindType
{
	Find,
	FindInFiles,
	Replace,
	ReplaceFind,
	ReplaceAll
};

enum { MaxRecentFiles = 20 };

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

	friend class Workspace;
	friend class FindDialog;
	friend class MainToolbar;

public:
	explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

	QStringList updateMRU(const QString& recent);
	Workspace*	getWorkspace();
	void		initWorkspace();
	int			openEditor(const QString& filePath);
	QTabWidget*	getEditorTabs();
	CodeEditor*	getEditor(int which = -1);
	DockMan*	getDockMan();
	void		deleteAllEditorTabs();	// delete all CodeEditors. Ask before deleting changed editors.

	bool		validateExe(const QString& file, QTextEdit* statusArea = NULL);
	void		launchPovray(const QString& file);
	const QString& getPovProcessErrorMsg() { return(povProcessErrorMsg); }
#ifdef USE_WEBSOCKETS
	WsClient*	getWsClient() { return(wsClient); }
#endif
	void		sendPovrayMessage(const QString& msg);
	void		setPrefVersionWidget(QTextEdit* w) { prefVersionWidget = w; }

	const PreferenceData& getPreferenceData() const { return(preferenceData); }
	const BookmarkMan*	getBookmarkMan() { return(m_bookmarkMan); }
	const FindMan*		getFindMan() { return(m_findMan); }
	SearchMan*			getSearchMan() { return(m_searchMan); }
	MainToolbar*		getToolbar() { return(m_mainToolbar); }
	InsertMenuMan*		getInsertMenuMan() { return(m_insertMenuMan); }

Q_SIGNALS:
	//void	povrayStatusChanged();
	void	emitStatusMessage(int stream, const QString& msg);
	void	emitNeedWorkspace();

public slots:
	void	editorModified(bool changed);
	void	closeCurrentEditor();
	void	closeEditorRequested(int which);
	void	saveCurrentEditor();
	void	saveAllEditors();
	void	tabChanged(int index);
	void	onPreferences();
	void	editIndentBlock();
	void	editUnindentBlock();
	void	editToggleComments();
	//void	onFilterDialog();
	void	povrayWsStateChanged(bool connected);
	void	wsMessageReceived(const QString& command, const QString& text);
	void	onRenderAction();
	void	onRenderStartIfNotRunning();
	void	needWorkspace();
	void	moveToEditor(const QString& file, int line, int col);

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	void	loadPreferences();
	void	savePreferences();
	bool	maybeSaveEditor(CodeEditor* ce);	// return true if ok to close
	QString findFile(const QString& file);		// Given a file name, find the full path
	void	setShortcutKeys();

#ifdef USE_WEBSOCKETS
	WsClient*		wsClient;
#endif
	QTextEdit*		prefVersionWidget;		// If prefs are open, this points to where the version is displayed
	QShortcut		m_shortcutConfigure;
	QShortcut		shortcutRender;
	QShortcut		shortcutNextError;
	QShortcut		shortcutPreviousError;
	QShortcut		shortcutBookmarkToggle;
	QShortcut		shortcutBookmarkNext;
	QShortcut		shortcutBookmarkPrevious;
	QShortcut		shortcutFindNext;
	QShortcut		shortcutFindPrevious;
	QShortcut		m_shortcutSaveAllEditors;
	QShortcut		m_shortcutEditCut;
	QShortcut		m_shortcutEditCopy;
	QShortcut		m_shortcutEditPaste;
	QShortcut		m_shortcutEditSelectAll;
	QShortcut		m_shortcutEditShiftLeft;
	QShortcut		m_shortcutEditShiftRight;
	QShortcut		m_shortcutEditToggleComments;
	QShortcut		m_shortcutEditGotoLineNumber;
	QShortcut		m_shortcutEditGotoMatchingBrace;

	Ui::MainWindow* ui;
	VfeClient*		m_vfeClient;
	QTabWidget*		editorTabs;
	QSize			editorTabsCloseSize;
	DockMan*		m_dockMan;
	SearchMan*		m_searchMan;
	BookmarkMan*	m_bookmarkMan;
	FindMan*		m_findMan;
	InsertMenuMan*	m_insertMenuMan;
	MainToolbar*	m_mainToolbar;
	StatusBar*		statusBar;
	PreferenceData	preferenceData;
	QString			povProcessErrorMsg;
};

inline QTabWidget* MainWindow::getEditorTabs() { return(editorTabs); }
inline DockMan* MainWindow::getDockMan() { return(m_dockMan); }

#endif // _MAINWINDOW_H_