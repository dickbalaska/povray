/******************************************************************************
 * debuggerconsole.h - The Widget to control POV-Ray SDL debugging
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
#ifndef DEBUGGERCONSOLE_H
#define DEBUGGERCONSOLE_H

#include <QSplitter>
#include <QTabWidget>
#include <QListWidget>
#include <QStackedWidget>
#include <QTableWidget>

class QToolBar;

class Breakpoint;
class MainWindow;
class DebuggerPanel;

class DebuggerOptions : public QWidget
{
	Q_OBJECT
public:
	explicit DebuggerOptions(QTabWidget* parent, MainWindow* mainWindow);
private:
	MainWindow*	m_mainWindow;
};

class IncludeStack : public QListWidget
{
	Q_OBJECT
public:
	explicit IncludeStack(QTabWidget* parent, MainWindow* mainWindow);
	
private:
	MainWindow*	m_mainWindow;
};

class BreakpointsWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit BreakpointsWidget(QTabWidget* parent, MainWindow* mainWindow);
	void	addBreakpoint(Breakpoint* bp);
	void	removeBreakpoint(Breakpoint* bp);
private:
	MainWindow*	m_mainWindow;
};


/// The DebuggerConsole is the entire Tab, left and right sides
class DebuggerConsole : public QSplitter
{
	friend class DebuggerMan;
	Q_OBJECT
public:
	explicit DebuggerConsole(QTabWidget* parent, QStackedWidget* consoleBar, MainWindow* mainWindow);
	
signals:

private:
	DebuggerPanel*		m_debuggerPanel;
	QTabWidget*			m_debuggerTabs;
	DebuggerOptions*	m_debuggerOptions;
	IncludeStack*		m_includeStack;
	BreakpointsWidget*	m_breakpointsWidget;
	
	MainWindow*	m_mainWindow;
	
};

#endif // DEBUGGERCONSOLE_H
