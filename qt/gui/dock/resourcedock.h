/******************************************************************************
 * resourcedock.h - The dock widget on the left that displays the file tree
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

#ifndef RESOURCEDOCK_H
#define RESOURCEDOCK_H

#include <QDockWidget>
#include <QTreeView>
#include <QHeaderView>
#include <QMouseEvent>
#include <QFileSystemModel>

#include "../mainwindow.h"

class ResourceDock;

///////////////////////////////////////////////////////////////////////////////
/// \brief The TreeView class
/// The primary object in the ResourceDock
class TreeView : public QTreeView
{
	Q_OBJECT
public:
	explicit TreeView(ResourceDock * parent);
protected:
	virtual void contextMenuEvent(QContextMenuEvent* event);
	virtual void keyPressEvent(QKeyEvent *event);
//	virtual void mouseDoubleClickEvent(QMouseEvent *event);
private slots:
	void	itemActivated(const QModelIndex& index);
private:
	bool	removeDirectoryRecursively(const QString& dirName);

	ResourceDock* resourceDock;
};

///////////////////////////////////////////////////////////////////////////////
class TreeHeader : public QHeaderView
{
	Q_OBJECT
public:
	explicit TreeHeader(QWidget* parent);

protected:
	virtual void mousePressEvent(QMouseEvent *e);

};

///////////////////////////////////////////////////////////////////////////////
class MyFileSystemModel : public QFileSystemModel
{
	Q_OBJECT
public:
	explicit MyFileSystemModel(ResourceDock* parent = nullptr)
		: QFileSystemModel(nullptr), m_resourceDock(parent) {}
	QVariant data(const QModelIndex &index, int role) const;
private:
	ResourceDock* m_resourceDock;
};

///////////////////////////////////////////////////////////////////////////////
class ResourceDock : public QDockWidget
{
	Q_OBJECT

	friend class TreeView;
	friend class MyFileSystemModel;
	friend class Workspace;
	friend class FilterDialog;

public:
	explicit ResourceDock(MainWindow* parent, Qt::WindowFlags flags = nullptr);

	void	addDirectory(const QString& path);
	void	setRenderThis(const QString& path);
	const QString& getRenderDirectory() { return(m_renderDir); }
	const QString& getRenderFile()		{ return(m_renderFile); }

public slots:
	void	selectDirectory();
	void	doubleClicked(const QModelIndex& index);
	void	renderThis(bool);
	void	removeRenderTag(bool);
	void	newFolder(bool);
	void	newFile(bool);
	void	openFile(bool);
	void	filterDialog(bool);
//	void	filterThis(bool);
//	void	filterExt(bool);

private:
	void	setupFilters();
	QString getFileName(const QString& path, bool isFolder = false);

	TreeView	m_treeView;
	MyFileSystemModel* m_fileSystemModel;
	MainWindow* m_mainWindow;
	QModelIndex m_contextIndex;
	bool		m_softDir;		// not a user specified renderDir
	QString		m_renderDir;
	QString		m_renderFile;
	QString		m_filePath;		// full path to render file (for icon matching)
};

#endif // RESOURCEDOCK_H
