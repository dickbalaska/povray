/******************************************************************************
 * debuggerman.cpp - Manage the gui side of the debugger for qtpovray
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

#include <QFileInfo>

#include "editor/codeeditor.h"
#include "mainwindow.h"
#include "debuggerman.h"
#include "debuggerpanel.h"
#include "debuggerconsole.h"
#include "debuggermessages.h"

static const QString	s_dbg("dbg ");		// note the space

// The commands that we send to the debugger
//static const QString s_cont("cont");		// continue
//static const QString s_pause("pause");
//static const QString s_b("b");

//// The commands we receive from the debugger
//static const QString s_init("init");
//static const QString s_break("break");

static QRegularExpression symRegExp("(\\S+) (\\S+) (.*)");

DebuggerMan::DebuggerMan(MainWindow* mainWindow)
	: m_mainWindow(mainWindow)
{	
	m_currentParserLocation.m_valid = false;
	connect(this, SIGNAL(emitMoveToEditor(QString,int,int)), m_mainWindow, SLOT(moveToEditor(QString,int,int)));
	connect(this, SIGNAL(emitShowStatusBarMessage(QString)), m_mainWindow, SLOT(showStatusBarMessage(QString)));	
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
		if (bp->m_fileName == filePath && bp->m_lineNumber == lineNumber) {
			found = true;
			m_breakpoints.removeOne(bp);
			m_debuggerConsole->m_breakpointsWidget->removeBreakpoint(bp);
			break;
		}
	}
	if (!found) {
		Breakpoint* bp = new Breakpoint;
		bp->m_fileName = filePath;
		bp->m_lineNumber = lineNumber;
		m_breakpoints.append(bp);
		m_debuggerConsole->m_breakpointsWidget->addBreakpoint(bp);
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
	m_debuggerConsole->m_breakpointsWidget->addBreakpoint(bp);
}

/// Return a list of the line numbers of the breakpoints for this file
QList<int>	DebuggerMan::gatherBreakpoints(CodeEditor* ce)
{
	QList<int> ql;
	for(Breakpoint* bp : m_breakpoints) {
		if (ce->getFilePath() == bp->m_fileName)
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
		if (bm->m_fileName == ce->getFilePath()) {
			iter = m_breakpoints.erase(iter);
			delete bm;
		} else
			++iter;
	}
	QList<int>::const_iterator liter;
	for (liter = list.begin(); liter != list.end(); ++liter) {
		Breakpoint* bp = new Breakpoint();
		bp->m_fileName = ce->getFilePath();
		bp->m_lineNumber = *liter;
		bp->m_enabled = true;
		addBreakpoint(bp);
	}
}

void DebuggerMan::setState(DbgState ns)
{
	m_state = ns;
	bool playPause = false;
	if (ns == dsInit || ns == dsReady)
		playPause = true;
	bool runEnabled = false;
	if (ns == dsInit || ns == dsParsing || ns == dsReady)
		runEnabled = true;
	bool stopEnabled = false;
	if (ns == dsReady || ns == dsParsing || ns == dsRendering)
		stopEnabled = true;
	bool stepEnabled = false;
	if (ns == dsReady)
		stepEnabled = true;
	m_debuggerConsole->m_debuggerPanel->setButtonStates(playPause, runEnabled, stopEnabled, stepEnabled);
}

void DebuggerMan::onDebuggerStart()
{
	switch (m_state) {
	case dsInit:
		setState(dsStartup);
		m_mainWindow->onStartDebugger();
		break;
	case dsReady:
		sendContinue();
		break;
	default:
		sendPause();
		break;
	}

	//handleBreak("29 test.pov");
}
void DebuggerMan::onDebuggerStop()
{
	
}
void DebuggerMan::onDebuggerStep()
{	
}

void DebuggerMan::onUserAddedSymbol(const QString& text)
{
	QString s = QString("%1%2 %3").arg(s_dbg, s_w, text);
	m_mainWindow->sendPovrayMessage(s);	
}

void DebuggerMan::messageFromPovray(const QString& msg)
{
	QString command;
	QString data;
	int i = msg.indexOf(' ');
	if (i == -1)
		command = msg;
	else {
		command = msg.left(i);
		data = msg.mid(i+1);
	}
	if (command == s_init) {
		sendBreakpoints();
		sendWatches();
		sendContinue();
	} else if (command == s_break) {
		handleBreak(data);
	} else if (command == s_sym) {
		handleSym(data);
	} else {
		qCritical() << "DebuggerMan:unknown msg" << msg;
	}
}

void DebuggerMan::sendBreakpoints()
{
	QString s;
	for (Breakpoint* bp : m_breakpoints) {
		if (bp->m_enabled) {
			QFileInfo f(bp->m_fileName);
			s = QString("%1%2 %3 %4").arg(s_dbg).arg(s_b).arg(bp->m_lineNumber).arg(f.fileName());
			m_mainWindow->sendPovrayMessage(s);			
		}
	}
}

void DebuggerMan::sendWatches()
{
	QString s;
	for (const QString& w : m_watches) {
		
	}
}

void DebuggerMan::sendContinue()
{
	QString s = s_dbg;
	s += s_cont;
	setState(dsParsing);
	m_mainWindow->sendPovrayMessage(s);
}

void DebuggerMan::sendPause()
{
	QString s = s_dbg;
	s += s_pause;
	setState(dsParsing);
	m_mainWindow->sendPovrayMessage(s);
}

void DebuggerMan::handleBreak(const QString& data)
{
	int i = data.indexOf(' ');
	if (i == -1) {
		qCritical() << "DebuggerMan:handleBreak bad data" << data;
		return;
	}
	int line = data.left(i).toInt();
	QString fileName = data.mid(i+1);
	m_currentParserLocation.m_fileName = fileName;
	m_currentParserLocation.m_lineNumber = line;
	m_currentParserLocation.m_valid = true;
	setState(dsReady);
	emit(emitMoveToEditor(fileName, line, 0));
	QString s = QString("Break at %1:%2").arg(fileName).arg(line);
	emit(emitShowStatusBarMessage(s));
}

void DebuggerMan::handleSym(const QString& data)
{
	QRegularExpressionMatch match = symRegExp.match(data);
	QStringList captured = match.capturedTexts();
	if (captured.length() != 4) {
		qCritical() << "Bad Sym command:" << data;
		return;
	}
	m_debuggerConsole->m_symbolsWidget->addSymbol(captured[1], captured[2], captured[3]);
}
