/******************************************************************************
 * consoledock.cpp - The dock widget where povray writes his text
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

#include <QDockWidget>
#include <QTabWidget>
#include <QTabBar>
#include <QHBoxLayout>
#include <QToolButton>
#include <QTextCursor>
#include <QTextBlock>
#include <QToolButton>
#include <QStackedWidget>
#include <QDebug>

#include "workspace.h"
#include "dropdown.h"
#include "searchman.h"
#include "consoledock.h"

ConsoleDock::ConsoleDock(MainWindow* parent, Qt::WindowFlags flags)
	: QDockWidget(QString("Consoles"), parent, flags), m_mainWindow(parent)
{
	setObjectName("ConsoleDock");

	m_consoleTabs = new QTabWidget(this);
	m_consoleBar = new QStackedWidget(m_consoleTabs);
	//m_consoleTabs->tabBar()->setMinimumHeight(140);
	m_povrayConsole = new PovrayConsole(m_consoleTabs, m_consoleBar, m_mainWindow);
	m_povrayConsole->setReadOnly(true);
	QFont font("Source Code Pro", 10);
	font.setFixedPitch(true);
	m_povrayConsole->setFont(font);
	connect(m_povrayConsole, SIGNAL(emitMoveToEditor(QString,int,int)), m_mainWindow, SLOT(moveToEditor(QString,int,int)));
	connect(m_mainWindow, SIGNAL(emitStatusMessage(int, QString)), m_povrayConsole, SLOT(addPovrayMessage(int,QString)));
	m_consoleTabs->addTab(m_povrayConsole, "POV-Ray");

	//m_searchView = new SearchTreeView(this);
	m_searchConsole = new SearchConsole(m_consoleTabs, m_consoleBar, m_mainWindow);
	m_consoleTabs->addTab(m_searchConsole, "Search Results");

	this->setWidget(m_consoleTabs);
	m_consoleTabs->setCornerWidget(m_consoleBar);
	m_consoleTabs->setCurrentIndex(0);
	m_consoleBar->setCurrentIndex(0);
	//connect(m_consoleTabs, SIGNAL(currentChanged(int)), )
	connect(m_consoleTabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
}

void ConsoleDock::configure(const PreferenceData* prefs)
{
	m_povrayConsole->configure(prefs);
}

void ConsoleDock::tabChanged(int which)
{
	qDebug() << "tabChanged";
	m_consoleBar->setCurrentIndex(which);
}

PovrayConsoleButtonBar::PovrayConsoleButtonBar(QStackedWidget* parent, PovrayConsole* console)
	: QWidget(parent)
{
	QStringList sl;
	sl << "unclassified" << "debug" << "info" << "statistic"
	   << "warning" << "error";
	QStringList tsl;
	tsl<< tr("Unclassified") << tr("Debug") << tr("Info") << tr("Statistics")
	   << tr("Warnings") << tr("Errors");

	this->setMinimumHeight(50);
	QHBoxLayout* layout = new QHBoxLayout(this);
	int left, top, right, bottom;
	layout->getContentsMargins(&left, &top, &right, &bottom);
	top = 0;
	layout->setContentsMargins(left, top, right, bottom);

	for (int i=0; i<_max; i++) {
		QString f = ":/resources/icons/show_" + sl[i] + ".png";
		m_buttons[i] = new QToolButton(this);
		m_buttons[i]->setIcon(QIcon(f));
		m_buttons[i]->setToolTip(tsl[i]);
		m_buttons[i]->setCheckable(true);
		connect(m_buttons[i], SIGNAL(toggled(bool)), console, SLOT(filterButtonToggled(bool)));
		layout->addWidget(m_buttons[i]);
	}
}

void PovrayConsoleButtonBar::setSelected(int selected)
{
	m_selected = selected;
	for (int i=0; i<_max; i++) {
		m_buttons[i]->setChecked(selected & (1<<i) ? true : false);
	}
	m_selected |= 1 << fatal;	// fatal is always on
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SearchConsole::SearchConsole(QTabWidget* parent, QStackedWidget *consoleBar, MainWindow* mainWindow)
	: SearchTreeView(parent),
	  m_mainWindow(mainWindow)
{
	m_dropdown = new Dropdown(consoleBar);
	connect(m_dropdown, SIGNAL(activated(QString)), this, SLOT(dropdownActivated(QString)));
	consoleBar->addWidget(m_dropdown);
}

void SearchConsole::setSearchTerm(const QString& text)
{
	m_itemModel->clear();
	m_itemModel->setHorizontalHeaderLabels(QStringList(text));
	m_dropdown->setValue(text);
}

void SearchConsole::dropdownActivated(const QString& text)
{
	m_mainWindow->getSearchMan()->findInFiles(text);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
PovrayConsole::PovrayConsole(QTabWidget* parent, QStackedWidget *consoleBar, MainWindow* mainWindow)
	: QPlainTextEdit(parent),
	  m_currentError(nullptr)
{
	m_mainWindow = mainWindow;
	m_parentTabs = parent;
	QFont font;
	font.setFamily("Source Code Pro");
	font.setFixedPitch(true);
	font.setPointSize(10);
	setFont(font);
	configure(&m_mainWindow->getPreferenceData());
	m_povrayConsoleButtons = new PovrayConsoleButtonBar(consoleBar, this);
	consoleBar->addWidget(m_povrayConsoleButtons);
	// indexed by ConsoleMsgType
	m_consoleColorList << "black" << "#6a5acd" << "blue" << "mediumvioletred"
					 << "orangered" << "firebrick" << "red" << "red";
	m_mapStreamToConsole << unclassified << debug << info << fatal
					   << unclassified << statistic << warning << error
					   << info;
}

PovrayConsole::~PovrayConsole()
{
	clearMessages();
}
void PovrayConsole::configure(const PreferenceData* prefs)
{
	setFont(prefs->getConsoleFont());
}

void PovrayConsole::addPovrayMessage(int stream, const QString& msg)
{
	//qDebug() << "msg:" << msg;
	ConsoleMsgType cmt = m_mapStreamToConsole[stream];
	ConsoleMessage* cm;
	bool showMe = false;
	if (stream == ERROR_STREAM) {
		cm = appendErrorMessage(msg);
		if (m_parentTabs->currentIndex() != 0)
			showMe = true;
	} else {
		cm = new ConsoleMessage(cmt, msg.toHtmlEscaped());
		m_consoleMessages.append(cm);
	}
	if (m_povrayConsoleButtons->m_selected & 1 << cmt || cm->m_type == fatal)
		displayConsoleMessage(cm);
	if (showMe) {
		m_parentTabs->setCurrentIndex(0);				// show the console on error
		QTextCursor tc(this->document());
		tc.movePosition(QTextCursor::End);
		this->setTextCursor(tc);
	}
}

void PovrayConsole::displayConsoleMessage(ConsoleMessage* cm)
{
	QString msg = "<font color=\"";
	msg += m_consoleColorList[cm->m_type] + "\"><pre>";
	if (cm->m_type == error)
		msg += "<b>" + cm->m_message + "</b>";
	else
		msg += cm->m_message;
	msg += "</pre></font>";
	if (!cm->m_file.isEmpty())
		msg += QString(tr("<font color=\"firebrick\"> at File: %1 Line: %2 Col: %3</font>"))
			   .arg(cm->m_file).arg(cm->m_line).arg(cm->m_col);
	appendHtml(msg);
}

void PovrayConsole::clearMessages()
{
	while (!m_consoleMessages.isEmpty()) {
		ConsoleMessage* cm = m_consoleMessages.takeFirst();
		delete cm;
	}
	clear();
	m_consoleMessages.clear();
	m_currentError = nullptr;
}

void PovrayConsole::redrawConsole()
{
	clear();
	foreach(ConsoleMessage* cm, m_consoleMessages) {
		if (m_povrayConsoleButtons->m_selected & 1 << cm->m_type || cm->m_type == fatal)
			displayConsoleMessage(cm);
	}
}

ConsoleMessage* PovrayConsole::appendErrorMessage(const QString& msg)
{
	bool debug=false;
	int i;
	for (i=1; i<msg.length(); i++) {	// first arg is filename wrapped in quotes
		if (msg[i] == '\\')				// with quotes escaped
			continue;
		if (msg[i] == '"')
			break;
	}
	QString file = msg.mid(1, i-1);
	if (debug) qDebug() << "File:" << file;
	int j = msg.indexOf(' ', i+2);
	QString lineT = msg.mid(i+2, j-i-2);
	if (debug) qDebug() << "lineT" << lineT;
	i = msg.indexOf(' ', j+1);
	QString colT = msg.mid(j+1, i-j-1);
	if (debug) qDebug() << "colT" << colT;
	QString text = msg.mid(i+1);
	if (debug) qDebug() << "text" << text;
	int line = lineT.toInt();
	int col = colT.toInt();
	ConsoleMessage* cm = new ConsoleMessage(error, text.toHtmlEscaped());
	cm->m_file = file;
	cm->m_line = line;
	cm->m_col = col;
	m_consoleMessages.append(cm);
	return(cm);
}

void PovrayConsole::mouseDoubleClickEvent(QMouseEvent* event)
{
	qDebug() << "mouseDoubleClickEvent" << event;
	QTextCursor c = cursorForPosition(event->pos());
	int line = c.blockNumber();
	if (/* DISABLES CODE */ (true)) {
		QString s = c.block().text();
		qDebug() << "line" << line << "text:" << s;
	}
	ConsoleMessage* cm = messageFromBlock(line);
	if (!cm->m_file.isEmpty())
		selectError(cm);
}

ConsoleMessage* PovrayConsole::messageFromBlock(int line)
{
	int index = -1;
	foreach(ConsoleMessage* cl, m_consoleMessages) {
		if (m_povrayConsoleButtons->m_selected & 1 << cl->m_type) {
			index++;
			if (cl->m_type == error)
				index++;
		}
		if (index >= line)
			return(cl);
	}
	return(nullptr);
}

int PovrayConsole::blockFromMessage(ConsoleMessage* cm)
{
	int index = -1;
	foreach(ConsoleMessage* cl, m_consoleMessages) {
		if (m_povrayConsoleButtons->m_selected & 1 << cl->m_type) {
			index++;
			if (cl->m_type == error)
				index++;
		}
		if (cm == cl)
			return(index);
	}
	return(-1);
}

void PovrayConsole::filterButtonToggled(bool checked)
{
	QObject* obj = sender();
	int c = checked ? 1 : 0;
	for (int i=0; i<_max; i++) {
		if (m_povrayConsoleButtons->m_buttons[i] == obj) {
			int a = m_povrayConsoleButtons->m_selected;
			a = (a & ~(1<<i)) | (c << i);
			m_mainWindow->getWorkspace()->setPovrayConsoleFilters(a);
			m_povrayConsoleButtons->m_selected = a;
		}
	}
	redrawConsole();

}

void PovrayConsole::selectError(ConsoleMessage* cm)
{
	if (true) {		// highlight the selected line in PovrayConsole
		int block = blockFromMessage(cm);
		QTextCursor c = textCursor();
		c.setPosition(0);
		c.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, block-1);
		this->setTextCursor(c);
		QList<QTextEdit::ExtraSelection> extraSelections;
		QTextEdit::ExtraSelection selection;
		QColor lineColor = QColor(Qt::yellow).lighter(160);
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		extraSelections.append(selection);
		setExtraSelections(extraSelections);
	}
	m_currentError = cm;
	emit(emitMoveToEditor(cm->m_file, cm->m_line, cm->m_col));
}
void PovrayConsole::previousError()
{
	ConsoleMessage* possibleMatch = nullptr;
	foreach(ConsoleMessage* cm, m_consoleMessages) {
		if (cm == m_currentError) {
			if (possibleMatch != nullptr) {
				selectError(possibleMatch);
				return;
			}
		}
		if (!cm->m_file.isEmpty()) {
			possibleMatch = cm;
		}
	}
}
void PovrayConsole::nextError()
{
	bool foundMe = false;
	foreach(ConsoleMessage* cm, m_consoleMessages) {
		if (!cm->m_file.isEmpty()) {
			if (m_currentError == nullptr) {
				selectError(cm);
				return;
			}
			if (foundMe) {
				selectError(cm);
				return;
			}
			if (cm == m_currentError)
				foundMe = true;
		}
	}
}

