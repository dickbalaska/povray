/******************************************************************************
 * mainwindow.cpp - The Qt QMainWindow for qtpovray
 *
 * qtpovray - A Qt GUI IDE frontend for POV-Ray
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

#include <QtWidgets>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "editor/codeeditor.h"
#include "editor/imagedisplayer.h"
#include "dock/dockman.h"
#include "dock/maintoolbar.h"
#include "dock/consoledock.h"
#include "dock/statusbar.h"
#include "dock/searchman.h"
#include "vfeclient.h"
#include "workspace.h"
#include "preferences.h"
#include "dock/filterdialog.h"
#include "editor/bookmarkman.h"
#include "findman.h"
#include "helpman.h"
#include "insertmenuman.h"
#include "qtpovrayversion.h"

QString	s_companyName = "qtpovray";		// directory to put the ini file in
QString	s_productName = "qtpovray";		// name of the ini file
QString s_recentWsList = "recentWorkspaces";

bool DEBUG = false;
QPixmap* mainPixmap;

QStringList streams = {"banner", "status", "debug", "fatal", "render", "statistic", "warning", "error"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	prefVersionWidget(NULL),
	m_shortcutConfigure(this),		shortcutRender(this),
	shortcutNextError(this),		shortcutPreviousError(this),
	shortcutBookmarkToggle(this),	shortcutBookmarkNext(this),
	shortcutBookmarkPrevious(this),	shortcutFindNext(this),
	shortcutFindPrevious(this),		m_shortcutSaveAllEditors(this),
	m_shortcutEditCut(this),		m_shortcutEditCopy(this),
	m_shortcutEditPaste(this),		m_shortcutEditSelectAll(this),
	m_shortcutEditShiftLeft(this),	m_shortcutEditShiftRight(this),
	m_shortcutEditToggleComments(this),
	m_shortcutEditGotoLineNumber(this),	m_shortcutEditGotoMatchingBrace(this),
	ui(new Ui::MainWindow)
{

    ui->setupUi(this);
#ifdef USE_WEBSOCKETS
	wsClient = NULL;
#endif
	m_editorTabs = NULL;
	CodeEditor::init();
	PovColor::init();
	setWindowTitle("qtpovray");
	mainPixmap = new QPixmap(":/resources/icons/Povray_logo_sphere.png");
	setWindowIcon(QIcon(*mainPixmap));
	connect(this, SIGNAL(emitNeedWorkspace()), this, SLOT(needWorkspace()), Qt::QueuedConnection);
	loadPreferences();
	m_bookmarkMan = new BookmarkMan(this);
	m_findMan = new FindMan(this);
	m_dockMan = new DockMan(this);
	m_helpMan = new HelpMan(this);
	m_mainToolbar = new MainToolbar("Main", preferenceData.getUseLargeIcons(), this);
	addToolBar(m_mainToolbar);
	m_mainToolbar->updateHelpEnabled();
	m_statusBar = new StatusBar(this);
	this->setStatusBar(m_statusBar);
	m_insertMenuMan = new InsertMenuMan(this);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, s_companyName, s_productName);
	bool b = Preferences::validateIns(preferenceData.getPovrayInsertMenu());
	m_mainToolbar->enableInsertMenu(b);
	m_insertMenuMan->populateMenu(b);

#ifdef USE_WEBSOCKETS
	m_mainToolbar->enableRender((validateExe(preferenceData.getPovrayExecutable())));
#endif
	QStringList files = settings.value(s_recentWsList).toStringList();
	if (!files.isEmpty())
		m_dockMan->activateWorkspace(files.first());
	else
		emit(this->emitNeedWorkspace());
	m_searchMan = new SearchMan(this);
	m_searchMan->setSearchConsole(m_dockMan->getConsoleDock()->getSearchConsole());
	m_shortcutConfigure.setContext(Qt::ApplicationShortcut);
	connect(&m_shortcutConfigure, SIGNAL(activated()), this, SLOT(onPreferences()));
	shortcutRender.setContext(Qt::ApplicationShortcut);
	connect(&shortcutRender, SIGNAL(activated()), this, SLOT(onRenderAction()));
	shortcutNextError.setContext(Qt::ApplicationShortcut);
	connect(&shortcutNextError, SIGNAL(activated()),
			m_dockMan->getConsoleDock()->getPovrayConsole(), SLOT(nextError()));
	shortcutPreviousError.setContext(Qt::ApplicationShortcut);
	connect(&shortcutPreviousError, SIGNAL(activated()),
			m_dockMan->getConsoleDock()->getPovrayConsole(), SLOT(previousError()));
	shortcutBookmarkToggle.setContext(Qt::ApplicationShortcut);
	connect(&shortcutBookmarkToggle, SIGNAL(activated()),
			m_bookmarkMan, SLOT(onBookmarkToggle()));
	shortcutBookmarkNext.setContext(Qt::ApplicationShortcut);
	connect(&shortcutBookmarkNext, SIGNAL(activated()),
			m_bookmarkMan, SLOT(onBookmarkNext()));
	shortcutBookmarkPrevious.setContext(Qt::ApplicationShortcut);
	connect(&shortcutBookmarkPrevious, SIGNAL(activated()),
			m_bookmarkMan, SLOT(onBookmarkPrevious()));
	shortcutFindNext.setContext(Qt::ApplicationShortcut);
	connect(&shortcutFindNext, SIGNAL(activated()),
			m_findMan, SLOT(onFindNext()));
	shortcutFindPrevious.setContext(Qt::ApplicationShortcut);
	connect(&shortcutFindPrevious, SIGNAL(activated()),
			m_findMan, SLOT(onFindPrevious()));
	m_shortcutSaveAllEditors.setContext(Qt::ApplicationShortcut);
	connect(&m_shortcutSaveAllEditors, SIGNAL(activated()),
			this, SLOT(saveAllEditors()));
	m_shortcutEditCut.setContext((Qt::ApplicationShortcut));
	m_shortcutEditCopy.setContext((Qt::ApplicationShortcut));
	m_shortcutEditShiftLeft.setContext(Qt::ApplicationShortcut);
	connect(&m_shortcutEditShiftLeft, SIGNAL(activated()),
			this, SLOT(editUnindentBlock()));
	m_shortcutEditShiftRight.setContext(Qt::ApplicationShortcut);
	connect(&m_shortcutEditShiftRight, SIGNAL(activated()),
			this, SLOT(editIndentBlock()));
	connect(&m_shortcutEditGotoMatchingBrace, SIGNAL(activated()),
			m_mainToolbar, SLOT(doGotoMatchingBrace()));
	connect(&m_shortcutEditGotoLineNumber, SIGNAL(activated()),
			m_mainToolbar, SLOT(doGotoLineNumber()));
	m_shortcutEditToggleComments.setContext(Qt::ApplicationShortcut);
//	connect(&m_shortcutEditToggleComments, SIGNAL(activated()),
//			this, SLOT(editToggleComments()));
	setShortcutKeys();
	m_vfeClient = new VfeClient(true, this);
	connect(m_vfeClient, SIGNAL(messageReceived(QString,QString)), this, SLOT(wsMessageReceived(QString,QString)));
	connect(m_vfeClient, SIGNAL(binaryMessageReceived(QByteArray)), m_dockMan->getRenderDock()->getRenderWidget(), SLOT(binaryMessageReceived(QByteArray)));
	qApp->installEventFilter(this);
}

MainWindow::~MainWindow() {
	delete m_vfeClient;
	delete m_searchMan;
	delete m_findMan;
	delete m_insertMenuMan;
    delete ui;
}

void MainWindow::setShortcutKeys()
{
	GlobalKeys* gk = preferenceData.getGlobalKeys();
	m_shortcutConfigure.setKey(gk->keyOpenPreferences);
	shortcutRender.setKey(gk->keyStartRender);
	shortcutNextError.setKey(gk->keyErrorNext);
	shortcutPreviousError.setKey(gk->keyErrorPrevious);
	shortcutBookmarkToggle.setKey(gk->keyBookmarkToggle);
	shortcutBookmarkNext.setKey(gk->keyBookmarkNext);
	shortcutBookmarkPrevious.setKey(gk->keyBookmarkPrevious);
	shortcutFindNext.setKey(gk->keyFindNext);
	shortcutFindPrevious.setKey(gk->keyFindPrevious);
	m_shortcutSaveAllEditors.setKey(gk->keySaveAll);
	m_shortcutEditShiftLeft.setKey(gk->keyUnindentBlock);
	m_shortcutEditShiftRight.setKey(gk->keyIndentBlock);
	m_shortcutEditToggleComments.setKey(gk->keyToggleComment);
	m_shortcutEditGotoLineNumber.setKey(gk->keyGotoLineNumber);
	m_shortcutEditGotoMatchingBrace.setKey(gk->keyGotoMatchingBrace);
}

bool MainWindow::eventFilter(QObject*, QEvent* e)
{
	switch (e->type()) {
	case QEvent::Shortcut: {
		QShortcutEvent* sev = static_cast<QShortcutEvent*>(e);
		if (sev->isAmbiguous()) {
			foreach(const auto& action, actions()) {
				if (action->shortcut() == sev->key()) {
					action->trigger(); // Trigger the action that matches the ambigous shortcut event.
					return true;
				}
			}
			return(true);
		}
	}
	// ...
	default: break;
	}

	return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (m_editorTabs) {
		for (int i=0; i<m_editorTabs->count(); i++) {
			CodeEditor* ce = getCodeEditor(i);
			if (ce && !maybeSaveEditor(ce)) {
				event->ignore();
				return;
			}
		}
	}
	savePreferences();
	m_dockMan->shutdown();
	event->accept();
}

Workspace* MainWindow::getWorkspace() { return(m_dockMan->getWorkspace()); }

void MainWindow::needWorkspace()
{
	bool ret = m_helpMan->showVirginSplash();
	if (!ret) {
		QApplication::quit();
		return;
	}
	emit(m_dockMan->emitSwitchWorkspace());
}

void MainWindow::initWorkspace()
{
	m_mainToolbar->enableSave(false);
}

int MainWindow::openEditor(const QString& filePath)
{
	int i;
	if (m_editorTabs == NULL) {
		m_editorTabs = new QTabWidget(this);
		m_editorTabs->setMinimumSize(400, 205);
		m_editorTabs->setTabsClosable(true);
		connect(m_editorTabs, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
		connect(m_editorTabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeEditorRequested(int)));
		setCentralWidget(m_editorTabs);

	}
	QWidget* qw;
	for (i=0; i<m_editorTabs->count(); i++) {
		qw = m_editorTabs->widget(i);
		CodeEditor* ce = qobject_cast<CodeEditor*>(qw);
		if (ce && ce->getFilePath() == filePath) {
			m_editorTabs->setCurrentIndex(i);
			return(i);
		}
		ImageDisplayer* id = qobject_cast<ImageDisplayer*>(qw);
		if (id && id->getFilePath() == filePath) {
			m_editorTabs->setCurrentIndex(i);
			return(i);
		}
	}
	QString fileName = filePath;
	i = filePath.lastIndexOf('/');
	if (i == -1)
		i = filePath.lastIndexOf('\\');
	if (i != -1)
		fileName = filePath.mid(i+1);

	EditorType et = determineEditorType(filePath, fileName);
	if (et == EditorTypeCode) {
		CodeEditor* ce;
		ce = new CodeEditor(this, &preferenceData);
		ce->setFilePath(filePath);
		ce->setFileName(fileName);

//		qDebug() << "openEditor ce=" << ce;
//		qDebug() << "openEditor filename=" << ce->getFileName();
//		qDebug() << "openEditor editorType:" << ce->getEditorType();

		int index = m_editorTabs->addTab(ce, fileName);
		m_editorTabs->setCurrentWidget(ce);
		editorTabsCloseSize = m_editorTabs->tabBar()->tabButton(index, QTabBar::RightSide)->size();
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			return(index);			// XXX report error
		QTextStream in(&file);
		QString s = in.readAll();
		ce->setPlainText(s);
		connect(ce, SIGNAL(modificationChanged(bool)), this, SLOT(editorModified(bool)));
		connect(ce, SIGNAL(copyAvailable(bool)), m_mainToolbar, SLOT(onCopyAvailable(bool)));
		connect(ce, SIGNAL(bookmarkCommand(int, int)), m_bookmarkMan, SLOT(onBookmarkCommand(int,int)));
		connect(ce, SIGNAL(bookmarkToggle(int)), m_bookmarkMan, SLOT(onBookmarkToggle(int)));
		connect(ce, SIGNAL(bookmarkNext(int)), m_bookmarkMan, SLOT(onBookmarkNext(int)));
		connect(ce, SIGNAL(bookmarkPrevious(int)), m_bookmarkMan, SLOT(onBookmarkPrevious(int)));
		ce->setBookmarks(m_bookmarkMan->gatherBookmarks(ce));
		m_editorTabs->widget(m_editorTabs->currentIndex())->setFocus();
		return(index);
	} else if (et == EditorTypeGraphic) {
		ImageDisplayer* id = new ImageDisplayer(this);
		id->setFilePath(filePath);
		id->setFileName(fileName);
		id->loadImage();
		int index = m_editorTabs->addTab(id, fileName);
		m_editorTabs->setCurrentWidget(id);
		return(index);
	}
	return(-1);
}

void MainWindow::closeCurrentEditor()
{
	closeEditorRequested(m_editorTabs->currentIndex());
}

void MainWindow::closeEditorRequested(int which)
{
	// qDebug() << "Removing tab" << which;
	CodeEditor* ce = getCodeEditor(which);
	if (ce) {
		maybeSaveEditor(ce);
		m_editorTabs->removeTab(which);
		delete ce;
	} else {
		QWidget* qw = getEditor(which);
		m_editorTabs->removeTab(which);
		delete qw;
	}
	// qDebug() << "m_editorTabs" << m_editorTabs->count();
	if (m_editorTabs->count() == 0) {
		delete m_editorTabs;
		m_editorTabs = NULL;
//		QLabel* ql = new QLabel(this);	// puts the icon in the middle,
//		ql->setPixmap(*mainPixmap);		// ugly because it resizes the tabs
//		this->setCentralWidget(ql);
	} else {
		m_editorTabs->widget(m_editorTabs->currentIndex())->setFocus();
	}

}
void MainWindow::deleteAllEditorTabs()
{
	if (m_editorTabs != NULL) {
		while (m_editorTabs->count()) {
			m_editorTabs->setCurrentIndex(0);
			CodeEditor* ce = (CodeEditor*)m_editorTabs->widget(0);
			maybeSaveEditor(ce);
			m_editorTabs->removeTab(0);
			delete ce;
		}
	}
	m_editorTabs = NULL;
//	QLabel* ql = new QLabel(this);
//	ql->setPixmap(*mainPixmap);
//	this->setCentralWidget(ql);
}

bool MainWindow::maybeSaveEditor(CodeEditor* ce)
{
	if (ce->isModified()) {
		QString s(tr("%1 is modified. Save?").arg(ce->getFilePath()));
		QMessageBox::StandardButton ret = QMessageBox::question(this, tr("File modified"), s,
							  QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel, QMessageBox::Yes);
		if (ret == QMessageBox::Yes)
			saveCurrentEditor();
		if (ret == QMessageBox::Cancel)
			return(false);
	}
	return(true);
}

void MainWindow::saveCurrentEditor()
{
	CodeEditor* ce = (CodeEditor*)m_editorTabs->widget(m_editorTabs->currentIndex());
	QString fname = ce->getFilePath();
	QFile file(fname);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QMessageBox::critical(this, tr("Error"), tr("Failed to save file"));
		return;		// XXX need better choices here
	}
	QString s = ce->toPlainText();
	file.write(s.toLocal8Bit());
	file.close();
	ce->document()->setModified(false);
}

void MainWindow::saveAllEditors()
{
	if (m_editorTabs) {
		for (int i=0; i<m_editorTabs->count(); i++) {
			CodeEditor* ce = (CodeEditor*)m_editorTabs->widget(i);
			if (ce->isModified()) {
				QString fname = ce->getFilePath();
				QFile file(fname);
				if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
					QMessageBox::critical(this, tr("Error"), tr("Failed to save file"));
					return;		// XXX need better choices here
				}
				QString s = ce->toPlainText();
				file.write(s.toLocal8Bit());
				file.close();
				ce->document()->setModified(false);
			}
		}
	}
}

QWidget *MainWindow::getEditor(int which)
{
	if (!m_editorTabs)
		return(nullptr);
	if (which == -1)
		return(m_editorTabs->widget(m_editorTabs->currentIndex()));
	else
		return(m_editorTabs->widget(which));
}

CodeEditor* MainWindow::getCodeEditor(int which)
{
	QWidget* widget = getEditor(which);
	CodeEditor* ce = qobject_cast<CodeEditor*>(widget);
	if (ce)
		return(ce);
	return(nullptr);
}
EditorType MainWindow::determineEditorType(const QString& filePath, const QString& )
{
	QImageReader qir(filePath);
	if (qir.canRead())
		return(EditorTypeGraphic);
	return(EditorTypeCode);
}

void MainWindow::tabChanged(int index)
{
	if (!m_editorTabs)
		return;
	for (int i=0; i<m_editorTabs->count(); i++) {
		if (m_editorTabs->tabBar()
				&& m_editorTabs->tabBar()->tabButton(i, QTabBar::RightSide)) {
// This doesn't work. Puts restored close boxes in the corner, not the middle
//			if (i == index)
//				editorTabs->tabBar()->tabButton(i, QTabBar::RightSide)->resize(16,32);
//			else
//				editorTabs->tabBar()->tabButton(i, QTabBar::RightSide)->resize(0,0);

// show/hide icon. This looks bad because the space isn't reclaimed. So just show the button then
//			if (i == index)
//				editorTabs->tabBar()->tabButton(i, QTabBar::RightSide)->show();
//			else
//				editorTabs->tabBar()->tabButton(i, QTabBar::RightSide)->hide();
		}
	}
	m_mainToolbar->tabChanged(index);
	setTitle(index);
}

void MainWindow::setTitle(int which)
{
	if (which == -1)
		setWindowTitle("qtpovray");
	else {
		QString mod = "";
		QString fname = "";
		CodeEditor* ce = getCodeEditor(which);
		if (ce) {
			fname = ce->getFileName();
			if (ce->isModified())
				mod = "*";
		} else {
			QWidget* qw = getEditor(which);
			ImageDisplayer* id = qobject_cast<ImageDisplayer*>(qw);
			if (id) {
				fname = id->getFileName();
			} else {
				fname = "***broken***";
			}
		}
		QString s = QString("%1%2 - qtpovray").arg(fname).arg(mod);
		setWindowTitle(s);
	}
}
QStringList MainWindow::updateMRU(const QString& recent)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, s_companyName, s_productName);
	QStringList files = settings.value(s_recentWsList).toStringList();
	files.removeAll(recent);
	files.prepend(recent);
	while (files.size() > MaxRecentFiles)
		files.removeLast();
	settings.setValue(s_recentWsList, files);
	m_mainToolbar->updateMRU(files);
	return(files);
}

void MainWindow::editorModified(bool changed)
{
	//qDebug() << "editorModified";
	CodeEditor* ce = (CodeEditor*)sender();
	bool found = false;
	int i;
	for (i=0; i<m_editorTabs->count(); i++) {
		if (m_editorTabs->widget(i) == ce) {
			found = true;
			break;
		}
	}
	if (!found) {
		qCritical() << "MainWindow::editorModified can't find CodeEditor for" << changed;
		return;
	}
	QString s = m_editorTabs->tabText(i);
	ce->setModified(changed);
	if (changed) {
		if (!s.endsWith(" *")) {
			s += " *";
			setTitle(i);
		}
	} else {
		if (s.endsWith(" *")) {
			s = s.left(s.size()-2);
			setTitle(i);
		}
	}
	m_editorTabs->setTabText(i, s);
	m_mainToolbar->enableSave(changed);
}

void MainWindow::moveToEditor(const QString& file, int line, int col)
{
	qDebug() << "moveToEditor" << file << line << col;
	QString path = findFile(file);
	if (path.isEmpty()) {
		QMessageBox::critical(this, tr("Can't find file"), QString(tr("Can't find file %1")).arg(file));
		return;
	}
	int index = openEditor(path);
	CodeEditor* ce = (CodeEditor*)m_editorTabs->widget(index);
	QTextCursor tc(ce->document()->findBlockByLineNumber(line-1));
	tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, col-1);
	ce->setTextCursor(tc);
	ce->centerCursor();
	ce->setFocus();
}

void MainWindow::editIndentBlock()
{
	qDebug() << "editIndentBlock";
	CodeEditor* ce = getCodeEditor();
	if (ce)
		ce->handleEditIndent();
}
void MainWindow::editUnindentBlock()
{
	qDebug() << "editUnindentBlock";
	CodeEditor* ce = getCodeEditor();
	if (ce)
		ce->handleEditUnindent();
}
void MainWindow::editToggleComments()
{
	qDebug() << "editToggleComments";
	CodeEditor* ce = getCodeEditor();
	if (ce)
		ce->handleEditToggleComments();
}

void MainWindow::onPreferences() {
	PreferenceData pd = preferenceData;

	Preferences p(this, &pd);
	//qDebug() << "MainColor" << pd.getEditorColors()->comment.getColor() << &pd.getEditorColors()->comment;
	int ret = p.exec();
	prefVersionWidget = NULL;
	if (ret == QDialog::Accepted) {
		bool changeIcons = false;
		if (preferenceData.getUseLargeIcons() != pd.getUseLargeIcons())
			changeIcons = true;
		preferenceData = pd;
		savePreferences();
		bool b = Preferences::validateIns(preferenceData.getPovrayInsertMenu());
		m_mainToolbar->enableInsertMenu(b);
		m_mainToolbar->updateHelpEnabled();
		m_insertMenuMan->populateMenu(b);
#ifdef USE_WEBSOCKETS
		m_mainToolbar->enableRender((validateExe(preferenceData.getPovrayExecutable())));
#endif
		if (m_editorTabs) {
			for (int i=0; i<m_editorTabs->count(); i++) {
				CodeEditor* ce = getCodeEditor(i);
				if (ce)
					ce->configure(&preferenceData);
			}
		}
		m_dockMan->getConsoleDock()->configure(&preferenceData);
		setShortcutKeys();
		if (changeIcons)
			m_mainToolbar->changeIcons(preferenceData.getUseLargeIcons());
	}
}

static QString s_EditorAutoBrace		("EditorAutoBrace");
static QString s_EditorAutoIndent		("EditorAutoIndent");
static QString s_EditorColors			("EditorColors");
static QString s_EditorHighlightLine	("EditorHighlightLine");
static QString s_EditorHighlightTokens	("EditorHighlightTokens");
static QString s_EditorTabWidth			("EditorTabWidth");
static QString s_EditorWrapText			("EditorWrapText");
static QString s_EditorFont				("EditorFont");
static QString s_ConsoleFont			("ConsoleFont");
static QString s_Keys					("Keys");
#ifdef USE_WEBSOCKETS
static QString s_PovrayExecutable		("PovrayExecutable");
#endif
static QString s_PovrayIncludes			("PovrayIncludes");
static QString s_PovrayInsertMenu		("PovrayInsertMenu");
static QString s_UserInsertMenu			("UserInsertMenu");
static QString s_PovrayHelpDirectory	("PovrayHelpDirectory");
static QString s_PovraySceneDirectory	("PovraySceneDirectory");
static QString s_Preferences			("Preferences");
static QString s_QtpovrayHelpDirectory	("QtpovrayHelpDirectory");
static QString s_UseLargeIcons			("UseLargeIcons");

void MainWindow::savePreferences() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, s_companyName, s_productName);
	settings.beginGroup(s_Preferences);
#ifdef USE_WEBSOCKETS
	settings.setValue(s_PovrayExecutable,	preferenceData.getPovrayExecutable());
#endif
	settings.setValue(s_PovrayIncludes,			preferenceData.getPovrayIncludes());
	settings.setValue(s_PovrayInsertMenu,		preferenceData.getPovrayInsertMenu());
	settings.setValue(s_UserInsertMenu,			preferenceData.getUserInsertMenu());
	settings.setValue(s_PovraySceneDirectory,	preferenceData.getPovraySceneDirectory());
	settings.setValue(s_PovrayHelpDirectory,	preferenceData.getPovrayHelpDirectory());
	settings.setValue(s_QtpovrayHelpDirectory,	preferenceData.getQtpovrayHelpDirectory());
	settings.setValue(s_EditorWrapText,			preferenceData.getEditorWrapText());
	settings.setValue(s_EditorTabWidth,			preferenceData.getEditorTabWidth());
	settings.setValue(s_EditorAutoIndent,		preferenceData.getAutoIndent());
	settings.setValue(s_EditorAutoBrace,		preferenceData.getAutoBraceCompletion());
	settings.setValue(s_EditorHighlightLine,	preferenceData.getEditorHighlightCurrentLine());
	settings.setValue(s_EditorHighlightTokens,	preferenceData.getEditorHighlightTokens());
	settings.setValue(s_EditorFont,				preferenceData.getEditorFont().toString());
	settings.setValue(s_ConsoleFont,			preferenceData.getConsoleFont().toString());
	settings.setValue(s_UseLargeIcons,			preferenceData.getUseLargeIcons());
	settings.endGroup();

#define writeColor(_highlight) \
	settings.setValue(#_highlight "Color", preferenceData.getEditorColorsNC()->_highlight.getColor()); \
	settings.setValue(#_highlight "Bold", preferenceData.getEditorColorsNC()->_highlight.isBold());

	settings.beginGroup(s_EditorColors);
	writeColor(common);
	writeColor(math);
	writeColor(modifier);
	writeColor(object);
	writeColor(texture);
	writeColor(color);
	writeColor(comment);
	writeColor(string);
	settings.endGroup();


#define writeKey(_key) \
	settings.setValue(#_key, preferenceData.getGlobalKeys()->_key.toString());

	settings.beginGroup(s_Keys);
	writeKey(keyOpenPreferences);
	writeKey(keyBookmarkToggle);
	writeKey(keyBookmarkNext);
	writeKey(keyBookmarkPrevious);
	writeKey(keyFindNext);
	writeKey(keyFindPrevious);
	writeKey(keyStartRender);
	writeKey(keyErrorNext);
	writeKey(keyErrorPrevious);
	settings.endGroup();
}

void MainWindow::loadPreferences() {
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, s_companyName, s_productName);
	settings.beginGroup(s_Preferences);
#ifdef USE_WEBSOCKETS
	preferenceData.setPovrayExecutable(settings.value(s_PovrayExecutable).toString());
#endif
	preferenceData.setPovrayIncludes(settings.value(s_PovrayIncludes).toString());
	preferenceData.setPovrayInsertMenu(settings.value(s_PovrayInsertMenu).toString());
	preferenceData.setUserInsertMenu(settings.value(s_UserInsertMenu).toString());
	preferenceData.setPovraySceneDirectory(settings.value(s_PovraySceneDirectory).toString());
	preferenceData.setPovrayHelpDirectory(settings.value(s_PovrayHelpDirectory).toString());
	preferenceData.setQtpovrayHelpDirectory(settings.value(s_QtpovrayHelpDirectory).toString());
	preferenceData.setEditorWrapText(settings.value(s_EditorWrapText, false).toBool());
	preferenceData.setEditorTabWidth(settings.value(s_EditorTabWidth, 4).toInt());
	preferenceData.setAutoIndent(settings.value(s_EditorAutoIndent, true).toBool());
	preferenceData.setAutoBraceCompletion(settings.value(s_EditorAutoBrace, true).toBool());
	preferenceData.setUseLargeIcons((settings.value(s_UseLargeIcons, true).toBool()));
	preferenceData.setEditorHighlightCurrentLine(settings.value(s_EditorHighlightLine, true).toBool());
	preferenceData.setEditorHighlightTokens(settings.value(s_EditorHighlightTokens, true).toBool());
	QString s = settings.value(s_EditorFont).toString();
	if (!s.isEmpty()) {
		QFont font;
		if (font.fromString(s))
			preferenceData.setEditorFont(font);
	}
	s = settings.value(s_ConsoleFont).toString();
	if (!s.isEmpty()) {
		QFont font;
		if (font.fromString(s))
			preferenceData.setConsoleFont(font);
	}
	settings.endGroup();

#ifdef USE_WEBSOCKETS
	if (preferenceData.getPovrayExecutable().isEmpty()) {
		QString dir = QCoreApplication::applicationDirPath();
		dir += "/povrayws";
#ifdef WIN32
        dir += "64.exe";
#endif
		QFile file(dir);
		if (file.exists())
			preferenceData.setPovrayExecutable(dir);
	}
#endif
	if (preferenceData.getPovrayInsertMenu().isEmpty()) {
		QString s = findPath("Insert Menu");
		preferenceData.setPovrayInsertMenu(s);
	}
	if (preferenceData.getPovrayIncludes().isEmpty()) {
		QString s = findPath("include");
		preferenceData.setPovrayIncludes(s);
	}
	if (preferenceData.getPovraySceneDirectory().isEmpty()) {
		QString s = findPath("scenes");
		preferenceData.setPovraySceneDirectory(s);
	}
	if (preferenceData.getQtpovrayHelpDirectory().isEmpty()) {
		QString s = findPath("qtpovrayHelp");
		preferenceData.setQtpovrayHelpDirectory(s);
	}
	if (preferenceData.getPovrayHelpDirectory().isEmpty()) {
		QString s = findPath("html");
		preferenceData.setPovrayHelpDirectory(s);
	}

#define readColor(_highlight, _defaultColor, _defaultBold) \
	preferenceData.getEditorColorsNC()->_highlight.setColor( \
	settings.value(#_highlight "Color", _defaultColor).value<QColor>()); \
	preferenceData.getEditorColorsNC()->_highlight.setBold( \
	settings.value(#_highlight "Bold", _defaultBold).toBool()); \

	settings.beginGroup(s_EditorColors);
	readColor(common, QColor(Qt::darkMagenta), false);
	readColor(math, QColor(Qt::darkRed), false);
	readColor(modifier, QColor(Qt::darkMagenta), false);
	readColor(object, QColor(Qt::darkYellow), true);
	readColor(texture, QColor(Qt::darkYellow), false);
	readColor(color, QColor(Qt::red), false);
	readColor(comment, QColor(Qt::darkGreen), false);
	readColor(string, QColor(Qt::darkBlue), false);
	settings.endGroup();

#define readKey(_key) \
	if (settings.value(#_key).isValid()) \
	{ preferenceData.getGlobalKeys()->_key = QKeySequence(settings.value(#_key).toString()); }

	settings.beginGroup(s_Keys);
	readKey(keyOpenPreferences);
	readKey(keyBookmarkToggle);
	readKey(keyBookmarkNext);
	readKey(keyBookmarkPrevious);
	readKey(keyFindNext);
	readKey(keyFindPrevious);
	readKey(keyStartRender);
	readKey(keyErrorNext);
	readKey(keyErrorPrevious);
	settings.endGroup();
	setShortcutKeys();

}

QString MainWindow::findPath(const QString& in)
{
	qDebug() << "findPath:" << in;
	QString myDir = QString("qtpovray-%1").arg(ROOT_VERSION);
	QStringList sl = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
	foreach (QString s, sl) {
		qDebug() << "Check: " << s;
		QDir qdir(s);
		if (qdir.cdUp() && qdir.cd(myDir) && qdir.cd(in)) {
			return(qdir.absolutePath());
		}
	}
	return("");
}

//void delay( int millisecondsToWait )
//{
//	QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
//	while( QTime::currentTime() < dieTime )
//	{
//		QCoreApplication::processEvents( QEventLoop::AllEvents, 20 );
//	}
//}
#ifndef USE_WEBSOCKETS
//bool MainWindow::validateExe(const QString&, QTextEdit*) {
//	return(true);
//}
#else
bool MainWindow::validateExe(const QString &file, QTextEdit *statusArea) {
	if (getWsClient() && getWsClient()->isConnected()) {
		//connect(getWsClient(), SIGNAL(messageReceived(QString,QString)), this, SLOT(wsMessageReceived(QString,QString)));
		sendWsMessage("version");
		if (statusArea)
			statusArea->setText("Connecting to povray...");
		return(true);
	}
	launchPovray(file);
	if (statusArea) {
		if (!getPovProcessErrorMsg().isEmpty()) {
			QString s = "<font color=\"red\"><b>";
			s += getPovProcessErrorMsg();
			s += "</b></font>";
			statusArea->setText(s);
		}
		else
			statusArea->clear();
	}
	return(false);
}
#endif

void MainWindow::wsMessageReceived(const QString& command, const QString& text)
{
	//qDebug() << "wsMessageReceived: command: " << command << "text:" << text;
	if (command == "version") {
		if (prefVersionWidget)
			prefVersionWidget->setText(text);
		m_helpMan->setPovrayVersion(text);
		return;
	} else if (command == "done") {
		m_mainToolbar->renderButtonToStart();
		this->m_statusBar->showMessage("Done");
		this->m_statusBar->renderDone();
		m_dockMan->getRenderDock()->repaint();
		return;
	} else if (command == "fatal") {
		emit(emitStatusMessage(fatal, text));
		return;
	} else if (command == "stream") {
		int i = text.indexOf(' ');
		if (i == -1) {
			qCritical() << "Malformed wsMessageReceived: " << text;
			return;
		}
		QString streamT(text.left(i));
		QString msg(text.mid(i+1));
		int stream = -1;
		for (i=0; i<streams.size(); i++) {
			if (streams.at(i) == streamT) {
				stream = i;
				break;
			}
		}
		if (stream >= 0) {
			emit(emitStatusMessage(stream, msg));
			return;
		} else {
			stream = streamT.toInt();
			if (streamT == "0" || stream == 1 || stream == 8 || stream == 3) {
				emit(emitStatusMessage(stream, msg));
				return;
			} else if (streamT == "7") {
				// render progress
				if (msg.contains("(100%)"))		// Hacky McHackface. Force a picture redraw when we see 100%
					m_dockMan->getRenderDock()->repaint();	// (This should be handled by the render window itself)
				this->m_statusBar->showRenderMessage(msg);
				return;
			} else if (streamT == "6") {
				// animation status
				this->m_statusBar->showAnimationMessage(msg);
				return;
			}
			if (stream == -1) {
				qCritical() << "wsMessageReceived no stream: " << text;
				return;
			}
		}
		qWarning() << "unhandled stream message:" << "text:" << text << "streamT" << streamT;
		return;
	}
	qWarning() << "unhandled message: command:" << command << "text:" << text;
}

void MainWindow::sendPovrayMessage(const QString& msg)
{
	m_vfeClient->sendMessage(msg);
#ifdef USE_WEBSOCKETS
	if (wsClient) {
		wsClient->sendMessage(msg);
	}
#else

#endif
}

#if 0
void	MainWindow::launchPovray(const QString& file __attribute__ ((unused)))
{
#ifdef USE_WEBSOCKETS
	qDebug() << "launchPovray";
	if (wsClient) {
		wsClient->close();
		delete wsClient;
		wsClient = NULL;
	}
	if (file.startsWith("ws://")) {
		wsClient = new WsClient(QUrl(file), DEBUG, this);
		povProcessErrorMsg = "";
		connect(wsClient, SIGNAL(clientStateChanged(bool)), this, SLOT(povrayWsStateChanged(bool)));
		connect(wsClient, SIGNAL(messageReceived(QString,QString)), this, SLOT(wsMessageReceived(QString,QString)));
		connect(wsClient, SIGNAL(binaryMessageReceived(QByteArray)), m_dockMan->getRenderDock(), SLOT(binaryMessageReceived(QByteArray)));
		wsClient->connectToPovray();
		povProcessErrorMsg = "";
		return;
	}
	QFile f(file);
	if (!f.exists()) {
		povProcessErrorMsg = "file doesn't exist: ";
		povProcessErrorMsg += file;
		return;
	}
	wsClient = new WsClient(QUrl("ws://localhost:4401"), DEBUG, this);
	QProcess* p = new QProcess(this);
	p->start(file);
	bool b = p->waitForStarted(2000);
	if (!b) {
		povProcessErrorMsg = "povrayws failed to start";
		return;
	}
	povProcessErrorMsg = "";
	connect(wsClient, SIGNAL(clientStateChanged(bool)), this, SLOT(povrayWsStateChanged(bool)));
	connect(wsClient, SIGNAL(messageReceived(QString,QString)), this, SLOT(wsMessageReceived(QString,QString)));
	connect(wsClient, SIGNAL(binaryMessageReceived(QByteArray)), m_dockMan->getRenderDock(), SLOT(binaryMessageReceived(QByteArray)));
	wsClient->setPovrayProcess(p);
	wsClient->connectToPovray();

//	int sleepInterval = 20;
//	int waitForIt = 2000;
//	int i = 0;
//	while (i < waitForIt) {
//		if (wsClient->isConnected())
//			return(true);
//		//delay(20);
//		QThread::msleep(20);
//		i += sleepInterval;
//	}
#endif
}
#endif

void MainWindow::onRenderStartIfNotRunning()
{
	if (!m_mainToolbar->isRenderButtonStart())
		return;
	onRenderAction();
}

void MainWindow::onRenderAction()
{
	qDebug() << "onRenderAction";
	if (!m_mainToolbar->isRenderButtonStart()) {
		this->m_statusBar->showMessage(tr("Canceling render"));
		sendPovrayMessage("cancel");
		return;
	}
	saveAllEditors();
	this->m_statusBar->showMessage(tr("Start render"));
	m_dockMan->getConsoleDock()->getPovrayConsole()->clearMessages();
	QString cl = "render ";
	QString rdir;
//	if (!m_dockMan->getResourceDock()->getRenderDirectory().isEmpty())
//		rdir = m_dockMan->getResourceDock()->getRenderDirectory();
//	else
//		rdir = getWorkspace()->getDirRoots().first();
	if (!isSoftSelect() && !m_dockMan->getResourceDock()->getRenderDirectory().isEmpty()) {
		rdir = m_dockMan->getResourceDock()->getRenderDirectory();
	} else {
		rdir = getWorkspace()->getDirRoots().first();
	}
	QString f;
	if (!isSoftSelect())
		f = m_dockMan->getResourceDock()->getRenderFile();
	else {
		CodeEditor* ce = getCodeEditor();
		if (ce)
			f = ce->getFileName();
		QDir dir(ce->getFilePath());
		QFileInfo fi(dir.absolutePath());
		if (fi.isFile()) {
			dir.cdUp();
		}
		rdir = dir.absolutePath();
	}

	cl += rdir;

	if (!preferenceData.getPovrayIncludes().isEmpty()) {
		cl += " +L";
		cl += preferenceData.getPovrayIncludes();
	}
	cl += " ";
	QString rcl;
	rcl = m_mainToolbar->getRenderCL()->getValue();
	if (!f.isEmpty()) {
		rcl += " ";
		if (!f.endsWith(".ini"))
			rcl += "+I";
		rcl += f;

	}
	cl += rcl;
	qDebug() << "sendMessage: " << cl;
	m_mainToolbar->renderButtonToStop();
	m_statusBar->renderStart();
	QString d("chdir:  ");
	d += rdir;
	emit(emitStatusMessage(QTINFO_STREAM, d));
	d = "command: ";
	d += rcl;
	emit(emitStatusMessage(QTINFO_STREAM, d));
	m_vfeClient->sendMessage(cl);

}

QString MainWindow::getCurrentEditorPath()
{
	CodeEditor* ce = getCodeEditor();
	if (ce)
		return(ce->getFilePath());
	return("");
}

void MainWindow::povrayWsStateChanged(bool connected)
{
	m_statusBar->showMessage(connected ? tr("Ready") : tr("Not Ready"));
	m_mainToolbar->enableRender(connected);
	m_mainToolbar->renderButtonToStart();
}

QString MainWindow::findFile(const QString& file)
{
	QFileInfo fi(file);
	if (fi.exists())
		return(fi.absoluteFilePath());
	QString s = m_dockMan->getResourceDock()->getRenderDirectory();
	s += "/";
	s += file;
	fi.setFile(s);
	if (fi.exists())
		return(fi.absoluteFilePath());
	s = getWorkspace()->getDirRoot();
	s += "/" + file;
	fi.setFile(s);
	if (fi.exists())
		return(fi.absoluteFilePath());

	s = preferenceData.getPovrayIncludes() + "/" + file;
	fi.setFile(s);
	if (fi.exists())
		return(fi.absoluteFilePath());
	return("");
}

bool MainWindow::isSoftSelect()
{
	if (!m_dockMan->getResourceDock()->getRenderFile().isEmpty())
		return(false);
	QString rcl;
	rcl = m_mainToolbar->getRenderCL()->getValue();
	if (rcl.contains("+I"))
		return(false);
	if (rcl.contains("-I"))
		return(false);
	return(true);

}
