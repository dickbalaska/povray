/******************************************************************************
 * debuggerman.h - Manage the gui side of the debugger for qtpovray
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2019 - Dick Balaska, and BuckoSoft.
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
#ifndef DEBUGGERMAN_H
#define DEBUGGERMAN_H

#include <QObject>

class CodeEditor;
class DebuggerConsole;
class MainWindow;

/// A POV-Ray parser location, file and line number. We only deal with column 1
class DebuggerLocation
{
public:
	QString	m_pathName;
	int		m_lineNumber;
};

class Breakpoint : public DebuggerLocation
{
public:
	bool	m_enabled;
};

enum DbgState {
	dsInit,
	dsStartup,
	dsReady,
	dsParsing,
	dsRendering,
	dsStopping,
};

class DebuggerMan : public QObject
{
	Q_OBJECT
	friend class Workspace;
public:
	DebuggerMan(MainWindow* mainWindow);
	virtual ~DebuggerMan();

	void	setDebuggerConsole(DebuggerConsole* debuggerConsole) { m_debuggerConsole = debuggerConsole; setState(); }
	DebuggerConsole*	getDebuggerConsole() { return(m_debuggerConsole); }

	QList<int>	gatherBreakpoints(CodeEditor* ce);
	void		addBreakpoint(Breakpoint* bm);

	void		messageFromPovray(const QString& msg);
	void		setState(DbgState newState = dsInit);

public slots:
	void	onBreakpointToggle(int lineNumber = 0);
	void	onUpdateBreakpoints(const QList<int>& list);
	void	onDebuggerStart();
	void	onDebuggerStop();
	void	onDebuggerStep();
	
private:
	MainWindow*			m_mainWindow;
	DebuggerConsole*	m_debuggerConsole;
	DebuggerLocation	m_currentLocation;
	QList<Breakpoint*>	m_breakpoints;
	DbgState			m_state;
};

#endif // DEBUGGERMAN_H
