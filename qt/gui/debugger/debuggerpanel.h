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
#ifndef DEBUGGERPANEL_H
#define DEBUGGERPANEL_H

#include <QObject>
#include <QWidget>
#include <QToolBar>

class MainWindow;


class DebuggerPanel : public QWidget
{
	Q_OBJECT
public:
	explicit DebuggerPanel(QWidget *parent, MainWindow* mainWindow);
	virtual ~DebuggerPanel() override;
	
	void	setButtonStates(bool playPause, bool playEnabled, bool stopEnabled, bool stepEnabled);

signals:
	
private:
	QToolBar*	m_buttonBar;
	QAction*	m_debuggerAction;
	QAction*	m_stopAction;
	QAction*	m_stepAction;

	MainWindow*	m_mainWindow;
};

#endif // DEBUGGERPANEL_H
