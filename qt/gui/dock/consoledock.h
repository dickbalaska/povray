/******************************************************************************
 * consoledock.h - The dock widget where povray writes his text
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

#ifndef _CONSOLEDOCK_H_
#define _CONSOLEDOCK_H_

#include <QDockWidget>
#include <QPlainTextEdit>
#include <QStackedWidget>

#include "searchman.h"
#include "mainwindow.h"

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QToolButton;
QT_END_NAMESPACE

class ConsoleDock;
class PovrayConsole;
class SearchTreeView;
class Dropdown;

class SearchTreeView;

///////////////////////////////////////////////////////////////////////////////
enum ConsoleMsgType {
	unclassified,
	debug,
	info,
	statistic,
	warning,
	error,		// Note: Error takes two lines in the display, displays filename/line/col.
	_max,
	fatal = _max
};

class ConsoleMessage : public QObject
{
	Q_OBJECT
	friend class PovrayConsole;
public:
	ConsoleMessage() {}
	ConsoleMessage(ConsoleMsgType type, const QString& msg) {
		m_type = type; m_message = msg;
	}

private:
	ConsoleMsgType	m_type;
	QString			m_message;
	QString			m_file;
	int				m_line;
	int				m_col;
};

class PovrayConsoleButtonBar : public QWidget
{
	Q_OBJECT
	friend class PovrayConsole;
public:
	PovrayConsoleButtonBar(QStackedWidget* parent, PovrayConsole* console);
	void setSelected(int selected);

	QToolButton*	m_buttons[_max];
	int				m_selected;
};

class PovrayConsole : public QPlainTextEdit
{
	Q_OBJECT
public:
	PovrayConsole(QTabWidget* parent, QStackedWidget* consoleBar, MainWindow* mainWindow);
	virtual ~PovrayConsole();

	void configure(const PreferenceData* prefs);

Q_SIGNALS:
	void emitMoveToEditor(const QString& file, int line, int col);

public slots:
	void addPovrayMessage(int stream, const QString& msg);

	void redrawConsole();
	void clearMessages();
	void nextError();
	void previousError();
	void filterButtonToggled(bool checked);
	PovrayConsoleButtonBar* getPovrayConsoleButtonBar() { return(m_povrayConsoleButtons); }

protected:
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
	ConsoleMessage* appendErrorMessage(const QString& msg);
	void			displayConsoleMessage(ConsoleMessage* cm);
	ConsoleMessage* messageFromBlock(int line);
	int				blockFromMessage(ConsoleMessage* cm);

	void selectError(ConsoleMessage* cm);

	//QList<ConsoleLink*>		consoleLinks;
	PovrayConsoleButtonBar*	m_povrayConsoleButtons;
	MainWindow*				m_mainWindow;
	QTabWidget*				m_parentTabs;
	QStringList				m_consoleColorList;
	QList<ConsoleMsgType>	m_mapStreamToConsole;
	QList<ConsoleMessage*>	m_consoleMessages;
	ConsoleMessage*			m_currentError;
};

class SearchConsole : public SearchTreeView
{
	Q_OBJECT
public:
	SearchConsole(QTabWidget* parent, QStackedWidget* consoleBar, MainWindow* mainWindow);

	void		setSearchTerm(const QString& text);

public slots:
	void		dropdownActivated(const QString& text);

private:
	MainWindow*		m_mainWindow;
	Dropdown*		m_dropdown;
};

///////////////////////////////////////////////////////////////////////////////
class ConsoleDock : public QDockWidget
{
	Q_OBJECT
public:
	explicit ConsoleDock(MainWindow* parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

	PovrayConsole* getPovrayConsole() { return(m_povrayConsole); }
	SearchConsole*	getSearchConsole() { return(m_searchConsole); }

	void	configure(const PreferenceData* prefs);

public slots:
	void	showConsole(int which);
	void	tabChanged(int which);
private:

	QTabWidget*				m_consoleTabs;
	QStackedWidget*			m_consoleBar;
	PovrayConsole*			m_povrayConsole;
	SearchConsole*			m_searchConsole;
	MainWindow*				m_mainWindow;

};

inline void ConsoleDock::showConsole(int which) { m_consoleTabs->setCurrentIndex(which); m_consoleBar->setCurrentIndex(which); }

#endif // _CONSOLEDOCK_H_
