/******************************************************************************
 * dockman.h - Manage the docking widgets for qtpov
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

#ifndef _DOCKMAN_H_
#define	_DOCKMAN_H_

//#include "../mainwindow.h"
#include "consoledock.h"
#include "resourcedock.h"
#include "renderdock.h"

class Workspace;
class MainWindow;

class DockMan : public QObject {
	Q_OBJECT

public:
	DockMan(MainWindow* mainWindow);
	virtual ~DockMan();
	void shutdown();

	void		activateWorkspace(const QString& pws);
	Workspace*	getWorkspace();		// return the current workspace
	ConsoleDock*	getConsoleDock() { return(m_consoleDock); }
	RenderDock*		getRenderDock() { return(m_renderDock); }
	ResourceDock*	getResourceDock() { return(m_resourceDock); }

signals:
	void	emitSwitchWorkspace();

public slots:
	void switchWorkspace();
	void openRecentPWS();
	void showAbout();

private:

	ConsoleDock*	m_consoleDock;
	ResourceDock*	m_resourceDock;
	RenderDock*		m_renderDock;
	MainWindow*		m_mainWindow;
	Workspace*		m_workspace;

};
inline Workspace* DockMan::getWorkspace() { return(m_workspace); }

#endif		// _DOCKMAN_H_
