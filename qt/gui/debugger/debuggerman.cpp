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
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "editor/codeeditor.h"
#include "mainwindow.h"
#include "debuggerman.h"
#include "debuggerpanel.h"
#include "debuggerconsole.h"
#include "debuggermessages.h"
#include "../source/parser/povdbgobjectnames.h"

static const QString	s_dbg("dbg");

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

/// CodeEditor's LineNumberArea has toggled a breakpoint on/off
void DebuggerMan::onBreakpointToggle(int lineNumber)
{
	CodeEditor* ce = dynamic_cast<CodeEditor*>(m_mainWindow->getEditor());
	if (!ce) {
//		qCritical() << "onBreakpointToggle from not a CodeEditor";
		return;
	}
	if (lineNumber == 0)
		lineNumber = ce->textCursor().blockNumber()+1;
	QString filePath = ce->getFilePath();
	bool found = false;
	for (Breakpoint* bp : m_breakpoints) {
		if (bp->m_filePath == filePath && bp->m_lineNumber == lineNumber) {
			found = true;
			m_breakpoints.removeOne(bp);
			m_debuggerConsole->m_breakpointsWidget->removeBreakpoint(bp);
			break;
		}
	}
	if (!found) {
		Breakpoint* bp = new Breakpoint;
		bp->m_filePath = filePath;
		bp->m_povrayFileName = getFileName(bp->m_filePath);
		bp->m_lineNumber = lineNumber;
		bp->m_enabled = true;
		m_breakpoints.append(bp);
		m_debuggerConsole->m_breakpointsWidget->addBreakpoint(bp);
	}
	QList<LineNumberBreakpoint> ql = gatherBreakpoints(ce);
	ce->setBreakpoints(ql);
	sendBreakpoints();
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

void DebuggerMan::onDeleteBreakpoint(const QString& filePath, int lineNumber)
{
	for (Breakpoint* bp : m_breakpoints) {
		if (bp->m_filePath == filePath && bp->m_lineNumber == lineNumber) {
			m_breakpoints.removeOne(bp);
			m_debuggerConsole->m_breakpointsWidget->removeBreakpoint(bp);
			CodeEditor* ce = m_mainWindow->getCodeEditor(filePath);
			if (ce) {
				QList<LineNumberBreakpoint> ql = gatherBreakpoints(ce);
				ce->setBreakpoints(ql);
			}
			break;
		}
	}	
}

/// Return a list of the line numbers of the breakpoints for this CodeEditor
QList<LineNumberBreakpoint>	DebuggerMan::gatherBreakpoints(CodeEditor* ce)
{
	QList<LineNumberBreakpoint> ql;
	LineNumberBreakpoint lnb;
	for(Breakpoint* bp : m_breakpoints) {
		if (ce->getFilePath() == bp->m_filePath) {
			lnb.mLineNumber = bp->m_lineNumber;
			lnb.mEnabled = bp->m_enabled;
			ql.append(lnb);
		}
	}
	return(ql);
}

/// CodeEditor has updated breakpoints
void DebuggerMan::onUpdateBreakpoints(const QList<LineNumberBreakpoint>& list)
{
	CodeEditor* ce = (CodeEditor*)sender();
	QList<Breakpoint*>::iterator iter = m_breakpoints.begin();
	while (iter != m_breakpoints.end()) {
		Breakpoint* bm = *iter;
		if (bm->m_filePath == ce->getFilePath()) {
			iter = m_breakpoints.erase(iter);
			delete bm;
		} else
			++iter;
	}

	for (const LineNumberBreakpoint& lnb : list) {
		Breakpoint* bp = new Breakpoint();
		bp->m_filePath = ce->getFilePath();
		bp->m_povrayFileName = getFileName(bp->m_filePath);
		bp->m_lineNumber = lnb.mLineNumber;
		bp->m_enabled = lnb.mEnabled;
		addBreakpoint(bp);		
	}
	sendBreakpoints();
}

void DebuggerMan::onBreakpointWidgetChanged(int row, int col)
{
	if (col != 0)
		return;
	bool b = m_debuggerConsole->m_breakpointsWidget->isActive(row);
	Breakpoint* bp = getBreakpoint(m_debuggerConsole->m_breakpointsWidget->getFilePath(row), 
								   m_debuggerConsole->m_breakpointsWidget->getLineNumber(row));
	if (!bp) {
		qCritical() << "onBreakpointWidgeetChanged panic: row/col" << row << "/" << col 
					<< m_debuggerConsole->m_breakpointsWidget->getFilePath(row) << ":" << m_debuggerConsole->m_breakpointsWidget->getLineNumber(row);
		return;
	}
	bp->m_enabled = b;
	m_mainWindow->breakpointsChanged(m_debuggerConsole->m_breakpointsWidget->getFilePath(row));
}

Breakpoint* DebuggerMan::getBreakpoint(const QString& filePath, int lineNumber)
{
	for (Breakpoint* bp : m_breakpoints) {
		if (bp->m_filePath == filePath && bp->m_lineNumber == lineNumber)
			return(bp);
	}
	return(nullptr);
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

void DebuggerMan::setRenderRootDirectory(const QString& dir)
{
	m_renderDir = dir;
	for (Breakpoint* bp : m_breakpoints) {
		bp->m_povrayFileName = getFileName(bp->m_filePath);
	}
}

QString DebuggerMan::getFileName(const QString& filePath)
{
	if (filePath.startsWith(m_renderDir)) {			// easy case. File is a straight up child of m_renderDir
		QString s = filePath.mid(m_renderDir.length()+1);
		return(s);
	}
	QFileInfo fi(filePath);
	if (fi.isAbsolute())
		return(filePath);
	qCritical() << "DebuggerMan:getFileName can't deal with" << filePath;
	qCritical() << "DebuggerMan:getFileName render root was" << m_renderDir;
	return(filePath);
}

QString DebuggerMan::getFilePath(const QString& fileName)
{
	QFileInfo fi(fileName);
	if (fi.isAbsolute())
		return(fileName);
	QDir dir(m_renderDir);
	QFileInfo fir(dir, fileName);
	return(fir.absolutePath());
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
	m_mainWindow->stopRendering();
	m_currentParserLocation.m_valid = false;
}

void DebuggerMan::onDebuggerStep()
{
	sendStep();
}

void DebuggerMan::onUserAddedSymbol(const QString& text)
{
	QJsonObject obj;
	obj[s_name] = text;
	m_debuggerConsole->m_symbolsWidget->addSymbol(obj);
	QString s = QString("%1 %2 %3").arg(s_dbg, s_w, text);
	m_mainWindow->sendPovrayMessage(s);
	if (!m_watches.contains(text))
		m_watches.append(text);
}

void DebuggerMan::onDeleteWatch(const QString& name)
{
	m_watches.removeOne(name);
	m_debuggerConsole->m_symbolsWidget->removeWatch(name);
	sendClearWatches();
	sendWatches();
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
	m_mainWindow->sendPovrayMessage(QString("%1 %2").arg(s_dbg, s_resetBreakpoints));
	for (Breakpoint* bp : m_breakpoints) {
		if (bp->m_enabled) {
			s = QString("%1 %2 %3 %4").arg(s_dbg).arg(s_b).arg(bp->m_lineNumber).arg(bp->m_filePath);
			m_mainWindow->sendPovrayMessage(s);			
		}
	}
}

void DebuggerMan::sendClearWatches()
{
	m_mainWindow->sendPovrayMessage(s_resetWatches);
}

void DebuggerMan::sendWatches()
{
	QString s;
	m_mainWindow->sendPovrayMessage(QString("%1 %2").arg(s_dbg, s_resetWatches));
	for (const QString& w : m_watches) {
		QString s = QString("%1 %2 %3").arg(s_dbg, s_w, w);
		m_mainWindow->sendPovrayMessage(s);	
	}
}

void DebuggerMan::sendContinue()
{
	QString s = QString("%1 %2").arg(s_dbg, s_cont);
	setState(dsParsing);
	m_mainWindow->sendPovrayMessage(s);
	m_currentParserLocation.m_valid  = false;
}

void DebuggerMan::sendStep()
{
	QString s = QString("%1 %2").arg(s_dbg, s_step);
	setState(dsParsing);
	m_mainWindow->sendPovrayMessage(s);
	m_currentParserLocation.m_valid  = false;
}

void DebuggerMan::sendPause()
{
	QString s = QString("%1 %2").arg(s_dbg, s_pause);
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
	m_currentParserLocation.m_povrayFileName = fileName;
	m_currentParserLocation.m_filePath = getFilePath(fileName);
	m_currentParserLocation.m_lineNumber = line;
	m_currentParserLocation.m_valid = true;
	setState(dsReady);
	emit(emitMoveToEditor(fileName, line, 0));
	QString s = QString("Break at %1:%2").arg(fileName).arg(line);
	emit(emitShowStatusBarMessage(s));
}

void DebuggerMan::handleSym(const QString& data)
{
	QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
	QJsonObject obj = doc.object();
	QString name = obj["name"].toString();
	QString type = obj[s_typeS].toString();
	QString value = obj[s_value].toString();
	m_debuggerConsole->m_symbolsWidget->addSymbol(obj);
//	QRegularExpressionMatch match = symRegExp.match(data);
//	QStringList captured = match.capturedTexts();
//	if (captured.length() != 4) {
//		qCritical() << "Bad Sym command:" << data;
//		return;
//	}
//	m_debuggerConsole->m_symbolsWidget->addSymbol(captured[1], captured[2], captured[3]);
}
