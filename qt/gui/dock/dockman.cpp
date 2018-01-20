/******************************************************************************
 * dockman.cpp - Manage the docking widgets for qtpov
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
#include <QMenuBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QScrollBar>
#include <QDebug>

#include "../mainwindow.h"
#include "../workspace.h"
#include "../editor/codeeditor.h"
#include "resourcedock.h"
#include "renderdock.h"
#include "dockman.h"
#include "version.h"


DockMan::DockMan(MainWindow* mainWindow)
	: m_mainWindow(mainWindow)
{
	m_workspace = NULL;
	m_consoleDock = new ConsoleDock(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_consoleDock);
	m_renderDock = new RenderDock(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::BottomDockWidgetArea, m_renderDock);
	m_resourceDock = new ResourceDock(m_mainWindow);
	m_mainWindow->addDockWidget(Qt::LeftDockWidgetArea, m_resourceDock);
	connect(this, SIGNAL(emitSwitchWorkspace()), this, SLOT(switchWorkspace()));
}

DockMan::~DockMan() {
	if (m_workspace)
		delete m_workspace;
	m_workspace = NULL;
}

void DockMan::shutdown() {
	if (m_workspace)
		delete m_workspace;
	m_workspace = NULL;
}

void DockMan::switchWorkspace() {
	QString p = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
	QDir d(p);
	if (!d.exists())
		d.mkpath(p);
	QString pws = QFileDialog::getSaveFileName(m_mainWindow, tr("Open Workspace"),
											   p,
											   tr("POV-Ray workspace files (*.pws)"),
											   NULL,
											   QFileDialog::DontConfirmOverwrite
											   );
	if (!pws.endsWith(".pws"))
		pws.append(".pws");
	activateWorkspace(pws);
}

void DockMan::openRecentPWS() {
	QAction *action = qobject_cast<QAction *>(sender());
	if (action) {
		QString pws = action->data().toString();
		activateWorkspace(pws);
	}
}

void DockMan::activateWorkspace(const QString& pws)
{
	m_mainWindow->deleteAllEditorTabs();
	qDebug() << "activeWorkspace: " << pws;
	QStringList mru = m_mainWindow->updateMRU(pws);

	if (m_workspace)
		delete m_workspace;
	m_workspace = new Workspace(m_mainWindow);
	m_workspace->load(pws);
	m_consoleDock->getPovrayConsole()->getPovrayConsoleButtonBar()->setSelected(m_workspace->getPovrayConsoleFilters());
	int index = m_workspace->getActiveEditorIndex();
	if (!m_workspace->getDirRoots().isEmpty())
		this->m_resourceDock->addDirectory(m_workspace->getDirRoots().first());
	if (!m_workspace->getOpenEditors().isEmpty()) {
		foreach (QString s, m_workspace->getOpenEditors()) {
			m_mainWindow->openEditor(s);
		}
		int positionCount = m_workspace->getEditorPositions().size();
		int scrollCount = m_workspace->getEditorScrolls().size();
		for (int i=0; i<m_mainWindow->getEditorTabs()->count(); i++) {
			CodeEditor* ce = m_mainWindow->getEditor(i);
			if (ce) {
				if (i < positionCount) {
					QVariant v = m_workspace->getEditorPositions().at(i);
					int p = v.toInt();
					QTextCursor tc = ce->textCursor();
					tc.setPosition(p);
					ce->setTextCursor(tc);
				}
				if (i < scrollCount) {
					QVariant v = m_workspace->getEditorScrolls().at(i);
					int p = v.toInt();
					QScrollBar* sb = ce->verticalScrollBar();
					sb->setValue(p);
				}
			}
		}
	}
	if (m_mainWindow->getEditorTabs())
		m_mainWindow->getEditorTabs()->setCurrentIndex(index);
	m_mainWindow->initWorkspace();
}

void DockMan::showAbout()
{
	QDialog d(m_mainWindow);
	d.setWindowTitle("About qtpov");
	QVBoxLayout* mainLayout = new QVBoxLayout(&d);
	QLabel* l = new QLabel(tr("qtpov - A multiplatform POV-Ray IDE"));
	mainLayout->addWidget(l);
	QString v(tr("Version %1").arg(VERSION));
#ifdef _DEBUG
	v += " - DEBUG";
#endif
	l = new QLabel(v);
	mainLayout->addWidget(l);
	QString compilationTime = QString("Built on: %1 %2").arg(__DATE__).arg(__TIME__);
	l = new QLabel(compilationTime);
	mainLayout->addWidget(l);
	l = new QLabel();
	l->setText("<a href=\"http://www.buckosoft.com/qtpov/\">www.buckosoft.com/qtpov</a>");
	l->setTextFormat(Qt::RichText);
	l->setTextInteractionFlags(Qt::TextBrowserInteraction);
	l->setOpenExternalLinks(true);	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	mainLayout->addWidget(l);
	connect(buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);
	mainLayout->addWidget(buttonBox);
	d.exec();
}
