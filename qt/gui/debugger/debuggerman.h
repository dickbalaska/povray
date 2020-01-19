/******************************************************************************
 * debuggerman.h - Manage the gui side of the debugger for qtpovray
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
#ifndef DEBUGGERMAN_H
#define DEBUGGERMAN_H

#include <QObject>
#include "editor/codeeditor.h"

class CodeEditor;
class DebuggerConsole;
class MainWindow;

/// A POV-Ray parser location, file and line number. We only deal with column 1
class DebuggerLocation
{
public:
	/// This is the filename given to the debugger.  Relative to the render startup directory.
	QString	m_povrayFileName;
	
	/// This is the full path to file, like what CodeEditor uses.
	QString	m_filePath;
	
	int		m_lineNumber;
};

class Breakpoint : public DebuggerLocation
{
public:
	bool	m_enabled;
};

/// Where the parser last stopped
class ParserLocation : public DebuggerLocation
{
public:
	bool	m_valid = false;
};

enum DbgState {
	dsInit,
	dsDone = dsInit,
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

	void	setDebuggerConsole(DebuggerConsole* debuggerConsole) { m_debuggerConsole = debuggerConsole; setState(dsInit); }
	DebuggerConsole*	getDebuggerConsole() { return(m_debuggerConsole); }

	QList<LineNumberBreakpoint>	gatherBreakpoints(CodeEditor* ce);
	void		addBreakpoint(Breakpoint* bm);

	void		messageFromPovray(const QString& msg);
	void		setState(DbgState newState = dsInit);
	const ParserLocation&	getParserLocation() {return(m_currentParserLocation); }
	Breakpoint*	getBreakpoint(const QString& filePath, int lineNumber);
	/// qtpovray works in full file paths.  POV-Ray works in file names relative to the root directory.
	void		setRenderRootDirectory(const QString& dir);

	// Given a file's full path, determine the fileName relative to the render root
	QString		getFileName(const QString& filePath);
	
	// Given a file name which may include a subdirectory, determine the full path with the render root
	QString		getFilePath(const QString& fileName);

Q_SIGNALS:
	void	emitMoveToEditor(const QString& file, int line, int col);
	void	emitShowStatusBarMessage(const QString& msg);
	
public slots:
	void	onBreakpointToggle(int lineNumber = 0);
	void	onUpdateBreakpoints(const QList<LineNumberBreakpoint>& list);

	/// Here when a user acts on the BreakpointWidget. Currently only enable/disable
	void	onBreakpointWidgetChanged(int row, int col);
	void	onDebuggerStart();
	void	onDebuggerStop();
	void	onDebuggerStep();
	void	onUserAddedSymbol(const QString& text);
	
private:
	void	sendBreakpoints();
	void	sendWatches();
	void	sendContinue();
	void	sendStep();
	void	sendPause();
	void	handleBreak(const QString& data);
	void	handleSym(const QString& data);

	MainWindow*			m_mainWindow;
	DebuggerConsole*	m_debuggerConsole;
	ParserLocation		m_currentParserLocation;
	QList<Breakpoint*>	m_breakpoints;
	QList<QString>		m_watches;
	DbgState			m_state;
	QString				m_renderDir;
};

#endif // DEBUGGERMAN_H
