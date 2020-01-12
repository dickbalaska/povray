/******************************************************************************
 * debuggerpanel.cpp - The left side of the Debugger console
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

#include "debuggerpanel.h"
#include "debuggerman.h"
#include "mainwindow.h"

static QIcon* playIcon;
static QIcon* pauseIcon;

DebuggerPanel::DebuggerPanel(QWidget *parent, MainWindow* mainWindow)
	: QWidget(parent),
	  m_mainWindow(mainWindow)
{
	QVBoxLayout* mainLayout = new QVBoxLayout();

	this->setStyleSheet("margin: 0 0 0 0; padding: 0 0 0 0; background:#EEEEEE;");
	m_buttonBar = new QToolBar(this);
	m_buttonBar->setStyleSheet("margin: 0 0 0 0; padding: 0 0 0 0; background:#EEEEEE;");
	QString sizeString;
	#define iString(name) QString(":/resources/icons/").append(name).append(sizeString)
	bool useLargeIcons = false;
	if (useLargeIcons) {
		m_buttonBar->setIconSize(QSize(32, 32));
		sizeString = "_32x32.png";
	} else {
		m_buttonBar->setIconSize(QSize(24, 24));
		sizeString = "_24x24.png";
	}
	
	playIcon = new QIcon(iString("Debug"));
	pauseIcon = new QIcon(iString("Pause"));
	
	DebuggerMan* dm = m_mainWindow->getDebuggerMan();
	m_debuggerAction = m_buttonBar->addAction(*playIcon,					"Debug", dm, SLOT(onDebuggerStart()));
	m_stopAction = m_buttonBar->addAction(QIcon(iString("Stop")),			"Stop",  dm, SLOT(onDebuggerStop()));
	m_stepAction = m_buttonBar->addAction(QIcon(iString("debugStepOver")),	"Step",  dm, SLOT(onDebuggerStep()));
	
	mainLayout->addWidget(m_buttonBar);
	mainLayout->addStretch(2);
	this->setLayout(mainLayout);
}

DebuggerPanel::~DebuggerPanel()
{
	delete playIcon;
	delete pauseIcon;
}

void DebuggerPanel::setButtonStates(bool playPause, bool playEnabled, bool stopEnabled, bool stepEnabled)
{
	if (playPause)
		m_debuggerAction->setIcon(*playIcon);
	else
		m_debuggerAction->setIcon(*pauseIcon);
	m_debuggerAction->setEnabled(playEnabled);
	m_stopAction->setEnabled(stopEnabled);
	m_stepAction->setEnabled(stepEnabled);
}
