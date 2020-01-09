#ifndef DEBUGGERMAN_H
#define DEBUGGERMAN_H

#include <QObject>

class CodeEditor;
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
	
};

class DebuggerMan : public QObject
{
	Q_OBJECT
	friend class Workspace;
public:
	DebuggerMan(MainWindow* mainWindow);
	virtual ~DebuggerMan();

	QList<int>	gatherBreakpoints(CodeEditor* ce);
	void		addBreakpoint(Breakpoint* bm);

public slots:
	void onBreakpointToggle(int lineNumber = 0);
	void onUpdateBreakpoints(const QList<int>& list);
	
private:
	MainWindow*			m_mainWindow;
	DebuggerLocation	m_currentLocation;
	QList<Breakpoint*>	m_breakpoints;
};

#endif // DEBUGGERMAN_H
