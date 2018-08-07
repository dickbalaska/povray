/******************************************************************************
 * workspace.cpp - metadata to define the state of the user's POV-Ray project
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

#include <QSettings>
#include <QComboBox>
#include <QMessageBox>
#include <QScrollBar>
#include <QDebug>

#include "mainwindow.h"
#include "dock/dockman.h"
#include "dock/maintoolbar.h"
#include "dock/consoledock.h"
#include "editor/bookmarkman.h"
#include "editor/codeeditor.h"
#include "editor/imagedisplayer.h"
#include "findman.h"

#include "workspace.h"

static QString s_aboutRect			("aboutRect");
static QString s_activeEditor		("activeEditor");
static QString s_bookmarks			("bookmarks");
static QString s_bmP				("bmP");
static QString s_bmL				("bmL");
static QString s_CL					("CL");
static QString s_colormapRect		("colormapRect");
static QString s_colorRect			("colorRect");
static QString s_colorModelessRect	("colorModelessRect");
static QString s_dirRoots			("dirRoots");
static QString s_editorPositions	("editorPositions");
static QString s_editorScrolls		("editorScrolls");
static QString s_fileOpenLocation	("fileOpenLocation");
static QString s_findPosition		("findPosition");
static QString s_findSize			("findSize");
static QString s_geometry			("geometry");
static QString s_MainWindow			("MainWindow");
static QString s_openEditors		("openEditors");
static QString s_povrayConsoleFilters("povrayConsoleFilters");
static QString s_prefsRect			("prefsRect");
static QString s_renderDockable		("renderDockable");
static QString s_renderFile			("renderFile");
static QString s_renderPos			("renderPos");
static QString s_resourceFilters	("resourceFilters");
static QString s_resourceFilterType	("resourceFilterType");
static QString s_state				("state");

Workspace::Workspace(MainWindow* mainWindow)
	: m_mainWindow(mainWindow)
{
	m_resourceFilters << "*.pov" << "*.inc" << "*.ini";
	m_resourceFilterType = FilterNone;
	m_povrayConsoleFilters =
			1 << unclassified |
			1 << debug |
			1 << info |
			1 << statistic |
			1 << warning |
			1 << error
			;
	m_renderRefreshDelay = 500;

}

Workspace::~Workspace() {
	save();
}

void Workspace::load(const QString& filename)
{
	qDebug() << "workspace::load '" << filename << "'";
	this->m_filename = filename;
	QSettings settings(filename, QSettings::IniFormat);
	settings.beginGroup(s_MainWindow);
	bool renderDockable = settings.value(s_renderDockable).toBool();
	QPoint p = settings.value(s_renderPos).toPoint();

	m_mainWindow->restoreGeometry(settings.value(s_geometry).toByteArray());
	m_mainWindow->restoreState(settings.value(s_state).toByteArray());

	RenderWidget* w = m_mainWindow->getDockMan()->getRenderWidget();
	qDebug() << "loading renderDockable" << renderDockable;
	if (!renderDockable) {
		if (!m_mainWindow->getDockMan()->getRenderWidget()->isDockable()) {
			RenderWindow* rw = w->getRenderWindow();
			qDebug() << "Already undocked";
			// already undocked, just set the pos
			rw->move(p);
		} else {
			// currently docked, make it undocked
			qDebug() << "Make it undocked";
			w->onDisableDockable();
			RenderWindow* rw = w->getRenderWindow();
			rw->move(p);
		}
	} else {
		// we are dockable. Check if we currently aren't
		bool curDockable = m_mainWindow->getDockMan()->getRenderWidget()->isDockable();
		qDebug() << "***Unhandled dockable: cur =" << curDockable;
	}
	settings.endGroup();

	QStringList slist = settings.value(s_dirRoots).toStringList();
	if (slist.isEmpty()) {
		QMessageBox::information(m_mainWindow, "Directory needed",
								 "In the following dialog, select the root of your POV-Ray source");
		m_mainWindow->m_dockMan->getResourceDock()->selectDirectory();
	} else {
		foreach (QString s, slist) {
			m_dirRoots.append(s);
		}
	}
	m_mainWindow->m_dockMan->getResourceDock()->setRenderThis(settings.value(s_renderFile, "").toString());
	slist = settings.value(s_openEditors).toStringList();
	foreach (QString s, slist) {
		m_openEditors.append(s);
	}
	m_editorPositions = settings.value(s_editorPositions).toList();
	m_editorScrolls = settings.value(s_editorScrolls).toList();
	m_activeEditorIndex = settings.value(s_activeEditor).toInt();
	m_fileOpenLocation = settings.value(s_fileOpenLocation, "").toString();
	m_resourceFilters = settings.value(s_resourceFilters, QStringList("*.pov") << "*.inc" << "*.ini").toStringList();
	m_resourceFilterType = (FilterFileStyle)settings.value(s_resourceFilterType, FilterNone).toInt();
	QVariant qv = settings.value(s_povrayConsoleFilters);
	if (qv.isValid())
		m_povrayConsoleFilters = qv.toInt();
	qv = settings.value(s_findPosition);
	if (qv.isValid())
		m_mainWindow->m_findMan->m_findDialogPos = qv.toPoint();
	qv = settings.value(s_findSize);
	if (qv.isValid())
		m_mainWindow->m_findMan->m_findDialogSize = qv.toSize();
	qv = settings.value(s_colormapRect);
	if (qv.isValid())
		m_colormapRect = qv.toRect();
	qv = settings.value(s_colorRect);
	if (qv.isValid())
		m_colorRect = qv.toRect();
	qv = settings.value(s_colorModelessRect);
	if (qv.isValid())
		m_colorModelessRect = qv.toRect();
	qv = settings.value(s_aboutRect);
	if (qv.isValid())
		m_aboutRect = qv.toRect();
	qv = settings.value(s_prefsRect);
	if (qv.isValid())
		m_prefsRect = qv.toRect();
	settings.beginGroup(s_CL);
	QComboBox* cb = m_mainWindow->m_mainToolbar->getRenderCL();
	cb->clear();
	cb->setInsertPolicy(QComboBox::InsertAtBottom);
	QString index;
	for (int i=0;; i++) {
		index.setNum(i);
		QString s = settings.value(index).toString();
		if (s.isNull())
			break;
		cb->addItem(s);
	}
	cb->setInsertPolicy(QComboBox::InsertAtTop);
	index.setNum(-1);
	QString s = settings.value(index).toString();
	cb->setCurrentText(s);
	settings.endGroup();

	settings.beginGroup(s_bookmarks);
	m_mainWindow->m_bookmarkMan->m_bookmarks.clear();
	for (int i=0;; i++) {
		index.setNum(i);
		QString s = settings.value(index).toString();
		if (s.isNull())
			break;
		int z = s.indexOf(' ');
		QString t = s.left(z);
		s = s.mid(z+1);
		Bookmark* bm = new Bookmark;
		bm->m_lineNumber = t.toInt();
		bm->m_pathName = s;
		m_mainWindow->m_bookmarkMan->addBookmark(bm);
	}
	settings.endGroup();
}

void Workspace::save()
{
	qDebug() << "workspace::save" << m_filename;
	QSettings settings(m_filename, QSettings::IniFormat);
	settings.beginGroup(s_MainWindow);
	settings.setValue(s_geometry, m_mainWindow->saveGeometry());
	settings.setValue(s_state, m_mainWindow->saveState());
	bool renderDockable = m_mainWindow->getDockMan()->getRenderWidget()->isDockable();
	settings.setValue(s_renderDockable, renderDockable);
	qDebug() << "saving renderDockable" << renderDockable;
	QPoint p;
	if (!renderDockable)
		p = m_mainWindow->getDockMan()->getRenderWidget()->getRenderWindow()->pos();
	settings.setValue(s_renderPos, p);
	settings.endGroup();

	QStringList slist(m_dirRoots);
	settings.setValue(s_dirRoots, slist);
	updateEditorPositions();
	QStringList dlist(m_openEditors);
	settings.setValue(s_openEditors, dlist);
	settings.setValue(s_editorPositions, m_editorPositions);
	settings.setValue(s_editorScrolls, m_editorScrolls);
	settings.setValue(s_activeEditor, m_activeEditorIndex);
	settings.setValue(s_fileOpenLocation, m_fileOpenLocation);
	settings.setValue(s_renderFile, m_mainWindow->m_dockMan->getResourceDock()->m_filePath);
	settings.setValue(s_resourceFilters, m_resourceFilters);
	settings.setValue(s_resourceFilterType, m_resourceFilterType);
	settings.setValue(s_povrayConsoleFilters, m_povrayConsoleFilters);
	settings.setValue(s_findPosition, m_mainWindow->m_findMan->m_findDialogPos);
	settings.setValue(s_findSize, m_mainWindow->m_findMan->m_findDialogSize);
	settings.setValue(s_colormapRect, m_colormapRect);
	settings.setValue(s_colorRect, m_colorRect);
	settings.setValue(s_colorModelessRect, m_colorModelessRect);
	settings.setValue(s_aboutRect, m_aboutRect);
	settings.setValue(s_prefsRect, m_prefsRect);
	settings.beginGroup(s_CL);
	QComboBox* cb = m_mainWindow->m_mainToolbar->getRenderCL();
	QString index;
#define writeCL(_i, _text) \
	index.setNum(_i); \
	settings.setValue(index, _text);

	writeCL(-1, cb->currentText());
	for (int i=0; i<cb->count(); i++) {
		writeCL(i, cb->itemText(i));
	}
	settings.endGroup();

	settings.beginGroup(s_bookmarks);
	settings.remove("");
	int i=0;
	QString s;
	foreach(Bookmark* bm, m_mainWindow->m_bookmarkMan->m_bookmarks) {
		index.setNum(i);
		s.setNum(bm->m_lineNumber);
		s += ' ';
		s += bm->m_pathName;
		settings.setValue(index, s);
		++i;
	}
	settings.endGroup();
}

void Workspace::addDirRoot(const QString &path)
{
	if (m_dirRoots.contains(path))
		return;
	m_dirRoots.append(path);
}

void Workspace::setResourceFilters(QStringList newFilters)
{
	this->m_resourceFilters = newFilters;
}
void Workspace::updateEditorPositions()
{
	m_openEditors.clear();
	m_editorPositions.clear();
	m_editorScrolls.clear();
	m_activeEditorIndex = 0;
	QTabWidget* tw = m_mainWindow->getEditorTabs();
	if (!tw)
		return;
	for (int i=0; i<tw->count(); i++) {
		QWidget* qw = tw->widget(i);
		CodeEditor* ce = qobject_cast<CodeEditor*>(qw);
		if (ce) {
			m_openEditors.append(ce->getFilePath());
			QTextCursor tc = ce->textCursor();
			m_editorPositions.append(QVariant(tc.position()));
			QScrollBar* sb = ce->verticalScrollBar();
			m_editorScrolls.append(QVariant(sb->value()));
		}
		ImageDisplayer* id = qobject_cast<ImageDisplayer*>(qw);
		if (id) {
			m_openEditors.append(id->getFilePath());
		}
	}
	m_activeEditorIndex = tw->currentIndex();
}
