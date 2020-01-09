#include "editor/codeeditor.h"
#include "debuggerman.h"

DebuggerMan::DebuggerMan(MainWindow* mainWindow)
	: m_mainWindow(mainWindow)
{
	
}

DebuggerMan::~DebuggerMan()
{
}

void DebuggerMan::onBreakpointToggle(int lineNumber)
{
	CodeEditor* ce = dynamic_cast<CodeEditor*>(sender());
	if (!ce) {
		qCritical() << "onBreakpointToggle from not a CodeEditor";
		return;
	}
	if (lineNumber == 0)
		lineNumber = ce->textCursor().blockNumber()+1;
	QString filePath = ce->getFilePath();
	bool found = false;
	for (Breakpoint* bp : m_breakpoints) {
		if (bp->m_pathName == filePath && bp->m_lineNumber == lineNumber) {
			found = true;
			m_breakpoints.removeOne(bp);
			break;
		}
	}
	if (!found) {
		Breakpoint* bp = new Breakpoint;
		bp->m_pathName = filePath;
		bp->m_lineNumber = lineNumber;
		m_breakpoints.append(bp);
	}
	QList<int> ql = gatherBreakpoints(ce);
	ce->setBreakpoints(ql);
}

void DebuggerMan::addBreakpoint(Breakpoint* bp)
{
	QList<Breakpoint*>::iterator iter;
	for (iter = m_breakpoints.begin(); iter != m_breakpoints.end(); ++iter) {
		Breakpoint* bp2 = *iter;
		if (bp->m_lineNumber > bp2->m_lineNumber) {
			continue;
		}
		m_breakpoints.insert(iter, bp);
		return;
	}
	m_breakpoints.append(bp);
}

QList<int>	DebuggerMan::gatherBreakpoints(CodeEditor* ce)
{
	QList<int> ql;
	for(Breakpoint* bp : m_breakpoints) {
		if (ce->getFilePath() == bp->m_pathName)
			ql.append(bp->m_lineNumber);
	}
	return(ql);
}

void DebuggerMan::onUpdateBreakpoints(const QList<int>& list)
{
	CodeEditor* ce = (CodeEditor*)sender();
	QList<Breakpoint*>::iterator iter = m_breakpoints.begin();
	while (iter != m_breakpoints.end()) {
		Breakpoint* bm = *iter;
		if (bm->m_pathName == ce->getFilePath()) {
			iter = m_breakpoints.erase(iter);
			delete bm;
		} else
			++iter;
	}
	QList<int>::const_iterator liter;
	for (liter = list.begin(); liter != list.end(); ++liter) {
		Breakpoint* bp = new Breakpoint();
		bp->m_pathName = ce->getFilePath();
		bp->m_lineNumber = *liter;
		addBreakpoint(bp);
	}
}
