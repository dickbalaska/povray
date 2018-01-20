/******************************************************************************
 * resourcedock.cpp - The dock widget on the left that displays the file tree
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

#include <QtWidgets>
#include <QDebug>

#include "../workspace.h"
#include "editor/codeeditor.h"
#include "filterdialog.h"
#include "resourcedock.h"

QPixmap* folderPm;
QPixmap* folder_selPm;
QPixmap* incPm;
QPixmap* inc_selPm;
QPixmap* iniPm;
QPixmap* ini_selPm;
QPixmap* povPm;
QPixmap* pov_selPm;

QStringList renderExtensions;

///////////////////////////////////////////////////////////////////////////////
ResourceDock::ResourceDock(MainWindow *parent, Qt::WindowFlags flags)
	: QDockWidget(QString("Resources"), parent, flags),
	  m_treeView(this),
	  m_mainWindow(parent),
	  m_softDir(false)
{
	setObjectName("ResourceDock");
	setWidget(&m_treeView);
	connect(&m_treeView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
	renderExtensions << "ini" << "inc" << "pov";

	folderPm	 = new QPixmap(":/resources/icons/folder.png");
	folder_selPm = new QPixmap(":/resources/icons/folder_sel.png");
	incPm		 = new QPixmap(":/resources/icons/povray_16x16.png");
	inc_selPm	 = new QPixmap(":/resources/icons/povray_16x16_sel.png");
	iniPm		 = new QPixmap(":/resources/icons/page_white_wrench.png");
	ini_selPm	 = new QPixmap(":/resources/icons/page_white_wrench_sel.png");
	povPm		 = new QPixmap(":/resources/icons/povray_16x16.png");
	pov_selPm	 = new QPixmap(":/resources/icons/povray_16x16_sel.png");
}

void ResourceDock::selectDirectory() {
	QString dir = QFileDialog::getExistingDirectory(m_mainWindow, tr("Select Directory"));
	if (dir.isEmpty())
		return;
	addDirectory(dir);
}

void ResourceDock::addDirectory(const QString &path)
{
	m_fileSystemModel = new MyFileSystemModel(this);
	m_fileSystemModel->setRootPath(path);
	setupFilters();
//    if (parser.isSet(dontUseCustomDirectoryIconsOption))
//        model.iconProvider()->setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);
	m_treeView.setModel(m_fileSystemModel);
	m_treeView.setRootIndex(m_fileSystemModel->index(path));
	m_treeView.hideColumn(1);
	m_treeView.hideColumn(2);
	m_treeView.hideColumn(3);
	m_treeView.model()->setHeaderData(0, Qt::Horizontal, path);
	QStandardItemModel* model1=new QStandardItemModel(0,1,this);

	model1->setHeaderData(0, Qt::Horizontal, path);
	//model1->setHeaderData(1, Qt::Horizontal, "header1");

	m_treeView.header()->setModel(model1);
	m_treeView.setRootIsDecorated(true);
//	m_treeView.setRootIndex(model.index(dir));
	m_treeView.show();
	Workspace* w = m_mainWindow->getWorkspace();
	w->addDirRoot(path);
}

void ResourceDock::setupFilters()
{
	if (m_mainWindow->getWorkspace()->getResourceFilterType() == FilterNone) {
		m_fileSystemModel->setNameFilters(QStringList("*"));
	} else {
		m_fileSystemModel->setNameFilters(m_mainWindow->getWorkspace()->getResourceFilters());
		m_fileSystemModel->setNameFilterDisables(
					m_mainWindow->getWorkspace()->getResourceFilterType() == FilterDim
					? true : false);
	}

}
void ResourceDock::doubleClicked(const QModelIndex &index)
{
	QString filePath = ((QFileSystemModel*)m_treeView.model())->filePath(index);
	qDebug() << "itemSelected:" << filePath;
	QFileInfo fi(filePath);
	if (fi.isDir())
		return;
	// XXX someday, figure out configurable editor types (.ini, graphics)
	m_mainWindow->openEditor(filePath);
	m_mainWindow->getEditor()->setFocus();
}

void ResourceDock::renderThis(bool )
{
	m_filePath = ((QFileSystemModel*)m_treeView.model())->filePath(m_contextIndex);
	setRenderThis(m_filePath);
}

void ResourceDock::removeRenderTag(bool )
{
	setRenderThis("");
}

void ResourceDock::setRenderThis(const QString& path)
{
	m_filePath = path;
	if (m_filePath.isEmpty()) {
		m_renderFile.clear();
		m_renderDir.clear();
		return;
	}
	QDir qd = QDir(path);
	m_renderFile = qd.dirName();
	qd.cdUp();
	m_renderDir = qd.absolutePath();
	//qDebug() << "renderThis:" << m_filePath << "dir:" << m_renderDir;
}

void ResourceDock::newFile(bool)
{
	QString path = ((QFileSystemModel*)m_treeView.model())->filePath(m_contextIndex);
	if (path.isEmpty())
		path = m_mainWindow->getWorkspace()->getDirRoot();
	qDebug() << "path" << path;
	QString fname = getFileName(path);
	if (!fname.isEmpty()) {
		QFile qf(path + "/" + fname);
		if (qf.exists()) {
			QMessageBox::warning(this, tr("Bad filename"), tr("File exists"));
			return;
		}
		bool b = qf.open(QIODevice::WriteOnly);
		if (!b) {
			QMessageBox::warning(this, tr("Error"), tr("Failed to create file"));
			return;
		}
		qf.close();
		m_mainWindow->openEditor(path + "/" + fname);
		m_mainWindow->getEditor()->setFocus();

	}
}

void ResourceDock::newFolder(bool)
{
	QString path = ((QFileSystemModel*)m_treeView.model())->filePath(m_contextIndex);
	if (path.isEmpty())
		path = m_mainWindow->getWorkspace()->getDirRoot();
	qDebug() << "path" << path;
	QString fname = getFileName(path, true);
	if (!fname.isEmpty()) {
		QDir qf(path + "/" + fname);
		if (qf.exists()) {
			QMessageBox::warning(this, tr("Bad directory name"), tr("Directory exists"));
			return;
		}
		bool b = qf.mkdir(qf.absolutePath());
		if (!b) {
			QMessageBox::warning(this, tr("Error"), tr("Failed to create directory"));
			return;
		}
	}

}

void ResourceDock::openFile(bool)
{
	QString p = m_mainWindow->getWorkspace()->getFileOpenLocation();
	QDir d(p);
	if (!d.exists())
		p = m_mainWindow->getWorkspace()->getDirRoot();
	QString ofname = QFileDialog::getOpenFileName(m_mainWindow, tr("Open File"), p);
	if (ofname.isNull())
		return;
	m_mainWindow->openEditor(ofname);
	d.setPath(ofname.left(ofname.lastIndexOf('/')));
	m_mainWindow->getWorkspace()->setFileOpenLocation(d.absolutePath());
	//qDebug() << "path:" << d.absolutePath();
}

void	ResourceDock::filterDialog(bool)
{
	FilterDialog fd(m_mainWindow, m_mainWindow->getWorkspace());
	fd.exec();
}

QString ResourceDock::getFileName(const QString& path, bool isFolder)
{
	QDialog d(this);
	d.setWindowTitle(tr("New File Name"));
	QVBoxLayout* mainLayout = new QVBoxLayout(&d);
	QString t = tr("Enter name for new file to create under");
	if (isFolder)
		t = tr("Enter name for new directory to create under");
	mainLayout->addWidget(new QLabel(t));
	mainLayout->addWidget((new QLabel(path)));
	QLineEdit* le = new QLineEdit(&d);
	mainLayout->addWidget(le);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										 | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, &d, &QDialog::reject);
	mainLayout->addWidget(buttonBox);
	int ret = d.exec();
	if (ret == QDialog::Accepted) {
		if (le->text().isEmpty()) {
			t = tr("Bad filename");
			QString m = tr("Filename can't be empty");
			if (isFolder) {
				t = tr("Bad directory name");
				m = tr("Directory name can't be empty");
			}
			QMessageBox::warning(this, t, m);
			return("");
		}
		return(le->text());
	}
	return("");
}

///////////////////////////////////////////////////////////////////////////////
/// \brief TreeHeader::TreeHeader
/// \param parent
/// This doesn't work. I wanted to grab a context click in order to display the
/// filter dialog, but the sizing of the subclassed widget is all wonky.
TreeHeader::TreeHeader(QWidget *parent)
	: QHeaderView(Qt::Horizontal, parent)
{

}

void TreeHeader::mousePressEvent(QMouseEvent *e)
{
	qDebug() << "TreeHeader.mouse" << e->button();
}

///////////////////////////////////////////////////////////////////////////////
TreeView::TreeView(ResourceDock* parent)
	: QTreeView(parent),
	  resourceDock(parent)

{
//	TreeHeader* th = new TreeHeader(parent);
//	this->setHeader(th);
	connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(itemActivated(QModelIndex)));
}

void TreeView::itemActivated(const QModelIndex& index)
{
	resourceDock->m_contextIndex = index;
}

void TreeView::contextMenuEvent(QContextMenuEvent* event)
{
	QModelIndex index = this->indexAt(event->pos());
	resourceDock->m_contextIndex = index;
	if (index.isValid()) {
		QString fpath = ((QFileSystemModel*)model())->filePath(index);
		QFileInfo finfo(fpath);
		if (finfo.isDir()) {
			QMenu menu(this);
			QMenu* fmenu = menu.addMenu("New");
			fmenu->addAction(tr("File"), resourceDock, SLOT(newFile(bool)));
			fmenu->addAction(tr("Folder"), resourceDock, SLOT(newFolder(bool)));
			menu.addSeparator();
			menu.addAction(tr("Resource Filters"), resourceDock, SLOT(filterDialog(bool)));
			menu.exec(event->globalPos());
			return;
		}
		QString fname = finfo.fileName();
		QString fext;
		int i = fname.lastIndexOf('.');
		if (i != -1) {
			fext =fname.mid(i+1);
		}
		qDebug() << "context filePath" << fpath;
		QMenu menu(this);
		if (fpath == resourceDock->m_filePath)
			menu.addAction("Remove render", resourceDock, SLOT(removeRenderTag(bool)));
		else {
			if (!fext.isEmpty() && renderExtensions.contains(fext))
				menu.addAction("Select for Render", resourceDock, SLOT(renderThis(bool)));
		}
		menu.addSeparator();
		menu.addAction(tr("Resource Filters"), resourceDock, SLOT(filterDialog(bool)));
		menu.exec(event->globalPos());
	} else {
		QMenu menu(this);
		QMenu* fmenu = menu.addMenu("New");
		fmenu->addAction(tr("File"), resourceDock, SLOT(newFile(bool)));
		fmenu->addAction(tr("Folder"), resourceDock, SLOT(newFolder(bool)));
		menu.addSeparator();
		menu.addAction(tr("Resource Filters"), resourceDock, SLOT(filterDialog(bool)));
		menu.addSeparator();
		menu.addAction(tr("Change Directory"), resourceDock, SLOT(selectDirectory()));
		menu.exec(event->globalPos());
	}
}

void TreeView::keyPressEvent(QKeyEvent *event)
{
	//qDebug() << "key:" << event->key();
	if (event->key() == Qt::Key_Delete) {
		qDebug() << "delete";
		QModelIndex index = resourceDock->m_contextIndex;
		if (index.isValid()) {
			QString fpath = ((QFileSystemModel*)model())->filePath(index);
			if (!fpath.isEmpty()) {
				QFileInfo fi(fpath);
				if (!fi.isDir()) {
					int ret = QMessageBox::question(this, tr("Delete file"), QString(tr("Delete file %1 ?")).arg(fpath));
					if (ret == QMessageBox::Yes) {
						QFile file(fpath);
						bool b = file.remove();
						if (!b) {
							QMessageBox::warning(this, tr("Error"), tr("Delete file failed."));
						}
					}
				} else {
					int ret = QMessageBox::question(this, tr("Delete directory"), QString(tr("Delete directory %1 ?")).arg(fpath));
					if (ret == QMessageBox::Yes) {
						bool b = removeDirectoryRecursively(fpath);
						if (!b) {
							QMessageBox::warning(this, tr("Error"), tr("Delete file failed."));
						}
					}

				}
			}
		}
		return;
	}
	QTreeView::keyPressEvent(event);
}

bool TreeView::removeDirectoryRecursively(const QString& dirName)
{
	bool result = true;
	QDir dir(dirName);

	if (dir.exists()) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = removeDirectoryRecursively(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = QDir().rmdir(dirName);
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////////
// Select the icon to display for this file in the TreeView
QVariant MyFileSystemModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DecorationRole) {
		QString fpath = filePath(index);
		//qDebug() << "msfm fpath:" << fpath;
		QFileInfo finfo(fpath);
		if (finfo.isDir()) {
			if (m_resourceDock->m_renderDir == fpath)
				return(*folder_selPm);
			return(*folderPm);
		}
		if (fpath.endsWith(".inc")) {
			if (m_resourceDock->m_filePath == fpath)
				return(*inc_selPm);
			return(*incPm);
		}
		if (fpath.endsWith(".ini")) {
			if (m_resourceDock->m_filePath == fpath)
				return(*ini_selPm);
			return(*iniPm);
		}
		if (fpath.endsWith(".pov")) {
			if (m_resourceDock->m_filePath == fpath)
				return(*pov_selPm);
			return(*povPm);
		}
	}
	return (QFileSystemModel::data(index, role));
}

