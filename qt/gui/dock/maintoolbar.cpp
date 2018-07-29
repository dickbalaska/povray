/******************************************************************************
 * maintoolbar.cpp - The primary toolbar for qtpovray
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
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

#include <QLineEdit>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QDialogButtonBox>

#include "../mainwindow.h"
#include "../preferences.h"
#include "../editor/codeeditor.h"
#include "../editor/bookmarkman.h"
#include "maintoolbar.h"
#include "dockman.h"
#include "helpman.h"

QIcon* renderGoIcon;
QIcon* renderStopIcon;
QString renderGoText;
QString renderStopText;


MainToolbar::MainToolbar(const QString &title, bool useLargeIcons, MainWindow *parent)
	: QToolBar(parent),
	  m_mainWindow(parent),
	  m_renderButtonIsStart(true)
{
	setWindowTitle(title);
	setObjectName(title);
	QString sizeString;
	#define iString(name) QString(":/resources/icons/").append(name).append(sizeString)

	if (useLargeIcons) {
		setIconSize(QSize(32, 32));
		sizeString = "_32x32.png";
	} else {
		setIconSize(QSize(24, 24));
		sizeString = "_24x24.png";
	}

	renderGoIcon = new QIcon(iString("Play"));
	renderStopIcon = new QIcon(iString("Stop"));
	renderGoText = tr("Start POV-Ray");
	renderStopText = tr("Stop POV-Ray");

	m_newAction = addAction(QIcon(iString("New")), "New", parent->getDockMan()->getResourceDock(), SLOT(newFile(bool)));
	m_openAction = addAction(QIcon(iString("Open")), "Open File",
						   m_mainWindow->getDockMan()->getResourceDock(), SLOT(openFile(bool)));
	m_openAction->setShortcut(QKeySequence::Open);
	m_saveAction = addAction(QIcon(iString("Save")), "Save", m_mainWindow, SLOT(saveCurrentEditor()));
	m_saveAction->setShortcut(QKeySequence::Save);
	m_saveAction->setDisabled(true);
	m_insertMenu = new QMenu("Insert", this);
	m_insertMenu->setIcon(QIcon(iString("InsertText")));
	addAction(m_insertMenu->menuAction());
	addAction(QIcon(iString("Settings")), "Settings", m_mainWindow, SLOT(onPreferences()));
	m_renderAction = addAction(*renderGoIcon, renderGoText, m_mainWindow, SLOT(onRenderAction()));
	renderCl = new Dropdown(this);
	connect(renderCl, SIGNAL(enterPressed()), m_mainWindow, SLOT(onRenderStartIfNotRunning()));
	addWidget(renderCl);
	setupMenu();
}

MainToolbar::~MainToolbar()
{
	for (int i=0; i< MaxRecentFiles; i++) {
		//recentFileActions[i]->disconnect();
		disconnect(recentFileConnections[i]);
		delete recentFileActions[i];
	}
}

void MainToolbar::changeIcons(bool useLargeIcons)
{
	QString sizeString;
	#define iString(name) QString(":/resources/icons/").append(name).append(sizeString)

	if (useLargeIcons) {
		setIconSize(QSize(32, 32));
		sizeString = "_32x32.png";
	} else {
		setIconSize(QSize(24, 24));
		sizeString = "_24x24.png";
	}
	renderGoIcon = new QIcon(iString("Play"));
	renderStopIcon = new QIcon(iString("Stop"));

	m_newAction->setIcon(QIcon(iString("New")));
	m_openAction->setIcon(QIcon(iString("Open")));
	m_saveAction->setIcon(QIcon(iString("Save")));
	m_insertMenu->setIcon(QIcon(iString("InsertText")));
	m_renderAction->setIcon(m_renderButtonIsStart ? *renderGoIcon : *renderStopIcon);
}

void MainToolbar::setupMenu()
{
	DockMan* dockMan = m_mainWindow->getDockMan();
	ResourceDock* resourceDock = dockMan->getResourceDock();
	QMenu* menu = m_mainWindow->menuBar()->addMenu(tr("&File"));

	QMenu* newM = new QMenu(tr("&New"), m_mainWindow);
	newM->addAction(QIcon(":resources/icons/New_24x24.png"), tr("&File"), resourceDock, SLOT(newFile(bool)));
	newM->addAction(QIcon(":resources/icons/folder.png"), tr("Fol&der"), resourceDock, SLOT(newFolder(bool)));
	menu->addMenu(newM);
	menu->addAction(m_openAction);
	menu->addAction(m_saveAction);
	m_saveAllAction = menu->addAction(tr("Save al&l"), m_mainWindow, SLOT(saveAllEditors()));
	m_saveAllAction->setShortcut(m_mainWindow->m_shortcutSaveAllEditors.key());
	m_saveAllAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	menu->addSeparator();
	QMenu* recentMenu = new QMenu(tr("Switch &Workspace"), m_mainWindow);
	for (int i=0; i< MaxRecentFiles; i++) {
		recentFileActions[i] = new QAction(this);
		recentFileActions[i]->setVisible(false);
		recentFileConnections[i] = connect(recentFileActions[i], SIGNAL(triggered()), m_mainWindow->getDockMan(), SLOT(openRecentPWS()));
		recentMenu->addAction(recentFileActions[i]);
	}
	recentMenu->addSeparator();
	recentMenu->addAction(tr("&Other"), m_mainWindow->getDockMan(), SLOT(switchWorkspace()));
	menu->addMenu(recentMenu);
	QAction* qa = menu->addAction(tr("&Quit"));
	connect(qa, SIGNAL(triggered()), m_mainWindow, SLOT(close()));

	/////////////////
	menu = m_mainWindow->menuBar()->addMenu(tr("&Edit"));
	m_editUndoAction = menu->addAction(tr("&Undo"));
	m_editUndoAction->setIcon(QIcon(":resources/icons/arrow_undo.png"));
	m_editUndoAction->setShortcut(QKeySequence::Undo);
	m_editUndoAction->setEnabled(false);
	connect(m_editUndoAction, SIGNAL(triggered(bool)), this, SLOT(doUndo()));
	m_editRedoAction = menu->addAction(tr("&Redo"));
	m_editRedoAction->setIcon(QIcon(":resources/icons/arrow_redo.png"));
	m_editRedoAction->setShortcut(QKeySequence::Redo);
	m_editRedoAction->setEnabled(false);
	connect(m_editRedoAction, SIGNAL(triggered(bool)), this, SLOT(doRedo()));
	menu->addSeparator();
	m_editCutAction = menu->addAction(tr("Cu&t"));
	m_editCutAction->setIcon(QIcon(":resources/icons/cut.png"));
	m_editCutAction->setShortcut(QKeySequence::Cut);
	m_editCutAction->setEnabled(false);
	connect(m_editCutAction, SIGNAL(triggered(bool)), this, SLOT(doCut()));
	m_editCopyAction = menu->addAction(tr("&Copy"));
	m_editCopyAction->setIcon(QIcon(":resources/icons/page_copy.png"));
	m_editCopyAction->setShortcut(QKeySequence::Copy);
	m_editCopyAction->setEnabled(false);
	connect(m_editCopyAction, SIGNAL(triggered(bool)), this, SLOT(doCopy()));
	m_editPasteAction = menu->addAction(tr("&Paste"));
	m_editPasteAction->setIcon(QIcon(":resources/icons/paste_plain.png"));
	m_editPasteAction->setShortcut(QKeySequence::Paste);
	connect(m_editPasteAction, SIGNAL(triggered(bool)), this, SLOT(doPaste()));

	menu->addSeparator();
	////////////////
	// Edit -> Advanced
	QMenu* advMenu = menu->addMenu(tr("&Advanced"));
	m_editAdvIndentAction = advMenu->addAction(tr("&Indent block"));
	m_editAdvIndentAction->setIcon(QIcon(":resources/icons/text_indent.png"));
	m_editAdvIndentAction->setShortcut(m_mainWindow->m_shortcutEditShiftRight.key());
	connect(m_editAdvIndentAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(editIndentBlock()));
	m_editAdvUnindentAction = advMenu->addAction(tr("&Unindent block"));
	m_editAdvUnindentAction->setIcon(QIcon(":resources/icons/text_indent_remove.png"));
	m_editAdvUnindentAction->setShortcut(m_mainWindow->m_shortcutEditShiftLeft.key());
	connect(m_editAdvUnindentAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(editUnindentBlock()));
	m_editAdvToggleComments = advMenu->addAction(tr("&Toggle comments"));
	m_editAdvToggleComments->setIcon(QIcon(":resources/icons/text_comments.png"));
	m_editAdvToggleComments->setShortcut(m_mainWindow->m_shortcutEditToggleComments.key());
	//m_editAdvToggleComments->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_BracketRight));
	connect(m_editAdvToggleComments, SIGNAL(triggered(bool)), m_mainWindow, SLOT(editToggleComments()));

	////////////////
	// Edit -> Goto
	// WTF, adding these to the menu kills the shortcut keys
//	QMenu* gotoMenu = menu->addMenu(tr("&Go to"));
//	m_editGotoLineNumber = gotoMenu->addAction(tr("&Line number"));
//	m_editGotoLineNumber->setShortcut(m_mainWindow->m_shortcutEditGotoLineNumber.key());
//	connect(m_editGotoLineNumber, SIGNAL(triggered(bool)), this, SLOT(doGotoLineNumber()));
//	m_editGotoMatchingBrace = gotoMenu->addAction(tr("&Matching Brace"));
//	m_editGotoMatchingBrace->setShortcut(m_mainWindow->m_shortcutEditGotoMatchingBrace.key());
//	connect(m_editGotoMatchingBrace, SIGNAL(triggered(bool)), this, SLOT(doGotoMatchingBrace()));
//	m_editGotoNextBookmark = gotoMenu->addAction(tr("&Next Bookmark"));
//	m_editGotoNextBookmark->setShortcut(m_mainWindow->shortcutBookmarkNext.key());
//	connect(m_editGotoNextBookmark, SIGNAL(triggered(bool)), m_mainWindow->getBookmarkMan(), SLOT(onBookmarkNext()));
//	m_editGotoPrevBookmark = gotoMenu->addAction(tr("&Previous Bookmark"));
//	m_editGotoPrevBookmark->setShortcut(m_mainWindow->shortcutBookmarkPrevious.key());
//	connect(m_editGotoPrevBookmark, SIGNAL(triggered(bool)), m_mainWindow->getBookmarkMan(), SLOT(onBookmarkPrevious()));

	menu->addSeparator();
	m_editPreferencesAction = menu->addAction("&Preferences");
	m_editPreferencesAction->setIcon(QIcon(":resources/icons/Settings_24x24.png"));
	m_editPreferencesAction->setShortcut(m_mainWindow->m_shortcutConfigure.key());
	connect(m_editPreferencesAction, SIGNAL(triggered(bool)), m_mainWindow, SLOT(onPreferences()));

	////////////////
	// Help
	menu = m_mainWindow->menuBar()->addMenu(tr("&Help"));	
	m_qtpovrayHelpAction = menu->addAction(tr("&qtpovray Help"), m_mainWindow->getHelpMan(), SLOT(showQtpovrayHelp()));
	m_povrayHelpAction = menu->addAction(tr("&Povray Help"), m_mainWindow->getHelpMan(), SLOT(showPovrayHelp()));
	menu->addSeparator();
	m_sampleScenesHelpAction = menu->addAction(tr("&Sample scenes"), m_mainWindow->getHelpMan(), SLOT(showSampleScenes()));
	menu->addAction(tr("&About qtpovray"), m_mainWindow->getHelpMan(), SLOT(showAbout()));
}

void MainToolbar::updateMRU(const QStringList& mru)
{
	int numRecentFiles = qMin(mru.size(), (int)MaxRecentFiles);
	for (int i=0; i<numRecentFiles; i++) {
		QString s = tr("&%1 %2").arg(QString::number(i+1), mru[i]);
		recentFileActions[i]->setText(s);
		recentFileActions[i]->setData(mru[i]);
		recentFileActions[i]->setVisible(true);
	}
	for (int j=numRecentFiles; j<MaxRecentFiles; j++)
		recentFileActions[j]->setVisible(false);
}

void MainToolbar::updateHelpEnabled()
{
	const PreferenceData& prefs = m_mainWindow->getPreferenceData();
	m_qtpovrayHelpAction->setEnabled(Preferences::validateQtpovrayHelpDirectory(prefs));
	m_povrayHelpAction->setEnabled(Preferences::validatePovrayHelpDirectory(prefs));
	m_sampleScenesHelpAction->setEnabled(Preferences::validatePovraySceneDirectory(prefs));
}

void  MainToolbar::renderButtonToStop()
{
	m_renderAction->setIcon(*renderStopIcon);
	m_renderAction->setToolTip(renderStopText);
	m_renderButtonIsStart = false;
}
void  MainToolbar::renderButtonToStart()
{
	m_renderAction->setIcon(*renderGoIcon);
	m_renderAction->setToolTip(renderGoText);
	m_renderButtonIsStart = true;
}

void MainToolbar::tabChanged(int index)
{
	//qDebug() << "tabChanged";
//	EditorBase* eb = m_mainWindow->getEditor(index);
	CodeEditor* ce = m_mainWindow->getCodeEditor(index);
	if (ce) {
		enableSave(ce->isModified());
		updateUndoRedo(ce);
		onCopyAvailable(ce->textCursor().hasSelection());
		ce->setFocus();
	} else {
		enableSave(false);
	}
}
void MainToolbar::updateUndoRedo(CodeEditor* ce)
{
	//qDebug() << "updateUndoRedo";
	if (ce) {
		m_editUndoAction->setEnabled(ce->isUndoAvailable());
		m_editRedoAction->setEnabled(ce->isRedoAvailable());
	}
}
void MainToolbar::enableSave(bool enable)
{
	m_saveAction->setEnabled(enable);
	if (!m_mainWindow->getEditorTabs()) {
		m_saveAllAction->setEnabled(false);
		return;
	}
	bool en = false;
	for (int i=0; i<m_mainWindow->getEditorTabs()->count(); i++) {
//		EditorBase* eb = m_mainWindow->getEditor(i);
//		if (eb && eb->getEditorType() == EditorTypeCode) {
		CodeEditor* ce = m_mainWindow->getCodeEditor(i);
		if (ce && ce->isModified())
			en = true;

	}
	m_saveAllAction->setEnabled(en);
}
void MainToolbar::enableInsertMenu(bool enable) {
	m_insertMenu->setEnabled(enable);
}

void MainToolbar::enableRender(bool enable) {
	m_renderAction->setEnabled(enable);
}

void MainToolbar::onCopyAvailable(bool yes)
{
	m_editCutAction->setEnabled(yes);
	m_editCopyAction->setEnabled(yes);
}
void MainToolbar::doCut()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce)
		ce->cut();
}

void MainToolbar::doCopy()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce)
		ce->copy();
}

void MainToolbar::doPaste()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce)
		ce->paste();
}

void MainToolbar::doUndo()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce)
		ce->undo();
}

void MainToolbar::doRedo()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce)
		ce->redo();
}

void MainToolbar::doGotoMatchingBrace()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce)
		ce->gotoMatchingBrace();
}

void MainToolbar::doGotoLineNumber()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (ce) {
		QDialog d(ce);
		d.setWindowTitle(tr("Go to line number"));
		QVBoxLayout* mainLayout = new QVBoxLayout(&d);
		QHBoxLayout* hLayout = new QHBoxLayout();
		mainLayout->addLayout(hLayout);
		hLayout->addWidget(new QLabel(tr("Line:")));
		QSpinBox* sb = new QSpinBox(this);
		int v = ce->textCursor().blockNumber()+1;
		sb->setRange(1,99999);
		sb->setValue(v);
		sb->selectAll();
		hLayout->addWidget(sb);
		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
											 | QDialogButtonBox::Cancel);
		connect(buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);
		connect(buttonBox, &QDialogButtonBox::rejected, &d, &QDialog::reject);
		mainLayout->addWidget(buttonBox);
		int ret = d.exec();
		if (ret == QDialog::Accepted) {
			ce->gotoLineNumber(sb->value());
		}
	}
}
