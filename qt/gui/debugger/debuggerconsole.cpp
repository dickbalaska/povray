/******************************************************************************
 * debuggerconsole.cpp - The Widget to control POV-Ray SDL debugging
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
#include <QVBoxLayout>
#include <QLabel>
#include <QToolBar>
#include <QHeaderView>
#include <QLineEdit>
#include <QDebug>

#include "debuggerpanel.h"
#include "debuggerconsole.h"
#include "debuggerman.h"
#include "mainwindow.h"

DebuggerConsole::DebuggerConsole(QTabWidget* parent, QStackedWidget* consoleBar, MainWindow* mainWindow)
	: QSplitter(Qt::Horizontal, parent),
	  m_mainWindow(mainWindow)
{
	m_debuggerPanel = new DebuggerPanel(this, mainWindow);
	this->addWidget(m_debuggerPanel);
	m_debuggerTabs = new QTabWidget(this);
	m_debuggerOptions = new DebuggerOptions(m_debuggerTabs, mainWindow);
	m_debuggerTabs->addTab(m_debuggerOptions, "Options");
	m_includeStack = new IncludeStack(m_debuggerTabs, mainWindow);
	m_debuggerTabs->addTab(m_includeStack, "IncStack");
	m_breakpointsWidget = new BreakpointsWidget(m_debuggerTabs, mainWindow);
	m_debuggerTabs->addTab(m_breakpointsWidget, "Breakpoints");
	m_symbolsWidget = new SymbolsWidget(m_debuggerTabs, mainWindow);
	m_debuggerTabs->addTab(m_symbolsWidget, "Symbols");
	
	this->addWidget(m_debuggerTabs);
	consoleBar->addWidget(new QLabel());	// just a dummy to blank the bar
}

IncludeStack::IncludeStack(QTabWidget* parent, MainWindow* mainWindow)
	: QListWidget(parent),
	  m_mainWindow(mainWindow)
{
	
}

DebuggerOptions::DebuggerOptions(QTabWidget* parent, MainWindow* mainWindow)
	: QWidget(parent),
	  m_mainWindow(mainWindow)
{
	
}

BreakpointsWidget::BreakpointsWidget(QTabWidget* parent, MainWindow* mainWindow)
	: QTableWidget(parent),
	  m_mainWindow(mainWindow)
{
	this->setColumnCount(3);
	setHorizontalHeaderLabels(QStringList({"Enabled", "#", "File"}));
	this->horizontalHeader()->setStretchLastSection(true);
	connect(this, SIGNAL(cellChanged(int, int)), this, SLOT(onBreakpointWidgetChanged(int, int)));
	connect(this, SIGNAL(breakpointChanged(int, int)), m_mainWindow->getDebuggerMan(), SLOT(onBreakpointWidgetChanged(int, int)));
}

void BreakpointsWidget::onBreakpointWidgetChanged(int row, int col) {
	if (m_initializing)
		return;
	emit(breakpointChanged(row, col));
}

void BreakpointsWidget::addBreakpoint(Breakpoint* bp)
{
	m_initializing = true;
	int row = this->rowCount();
	this->setRowCount(row+1);
	QTableWidgetItem* twi;
	twi = new QTableWidgetItem();
	twi->setCheckState(bp->m_enabled ? Qt::Checked : Qt::Unchecked);
	this->setItem(row, 0, twi);
	twi = new QTableWidgetItem(QString("%1").arg(bp->m_lineNumber));
	twi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	this->setItem(row, 1, twi);
	twi  = new QTableWidgetItem(bp->m_fileName);
	twi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	this->setItem(row, 2, twi);
	m_initializing = false;
}

void BreakpointsWidget::removeBreakpoint(Breakpoint* bp)
{
	for (int row = 0; row < this->rowCount(); row++) {
		QTableWidgetItem* twi = this->item(row, 2);
		if (twi->text() == bp->m_fileName) {
			twi = this->item(row, 1);
			int line = twi->text().toInt();
			if (line == bp->m_lineNumber) {
				this->removeRow(row);
				return;
			}
		}
	}
	qWarning() << "BreakpointsWidget: failed to remove Breakpoint" << bp->m_fileName << bp->m_lineNumber;
}

bool BreakpointsWidget::isActive(int row)
{
	QTableWidgetItem* twi = item(row, 0);
	return(twi->checkState() == Qt::Checked);
}

int BreakpointsWidget::getLineNumber(int row)
{
	QTableWidgetItem* twi = item(row, 1);
	if (!twi)
		return(-1);
	int l = twi->data(Qt::DisplayRole).toInt();
	return(l);
}

QString BreakpointsWidget::getFilename(int row)
{
	QTableWidgetItem* twi = item(row, 2);
	QString s = twi->data(Qt::DisplayRole).toString();
	return(s);
}

SymbolsWidget::SymbolsWidget(QTabWidget* parent, MainWindow* mainWindow)
	: QWidget(parent),
	  m_mainWindow(mainWindow)
{
	QVBoxLayout* mainLayout = new QVBoxLayout();
	m_table = new QTableWidget(this);
	m_table->setColumnCount(3);
	m_table->setHorizontalHeaderLabels(QStringList({"Name", "Type", "Value"}));
	m_table->horizontalHeader()->setStretchLastSection(true);
	mainLayout->addWidget(m_table);
	mainLayout->setStretchFactor(m_table, 100);
	m_lineEdit = new QLineEdit(this);
	connect(m_lineEdit, SIGNAL(returnPressed()),this, SLOT(onReturnPressed()));
	connect(this, SIGNAL(userSymbolAdded(const QString&)), m_mainWindow->getDebuggerMan(), SLOT(onUserAddedSymbol(const QString&)));
	mainLayout->addWidget(m_lineEdit);
	this->setLayout(mainLayout);
}

void SymbolsWidget::onReturnPressed()
{
	emit(userSymbolAdded(m_lineEdit->text()));
	m_lineEdit->clear();
}

void SymbolsWidget::addSymbol(const QString& name, const QString& type, const QString& value)
{
	QTableWidgetItem* twi;
	QList<QTableWidgetItem*> items = m_table->findItems(name, Qt::MatchFixedString | Qt::MatchCaseSensitive);

	int row = m_table->rowCount();
	m_table->setRowCount(row+1);
	twi = new QTableWidgetItem(name);
	m_table->setItem(row, 0, twi);
	twi = new QTableWidgetItem(type);
	m_table->setItem(row, 1, twi);
	twi = new QTableWidgetItem(value);
	m_table->setItem(row, 2, twi);
	
}
