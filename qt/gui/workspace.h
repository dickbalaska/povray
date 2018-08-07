/******************************************************************************
 * workspace.h - metadata to define the state of the user's POV-Ray project
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

#ifndef _WORKSPACE_H_
#define _WORKSPACE_H_

#include <QObject>
#include <QRect>
#include <QVariant>

class MainWindow;

enum FilterFileStyle {
	FilterNone,
	FilterDim,
	FilterHide
};


class Workspace : QObject
{
public:
	Workspace(MainWindow* mainWindow);
	virtual ~Workspace();

	void load(const QString& filename);
	void save();

	const QList<QString> getDirRoots() { return(m_dirRoots); }
	const QString& getDirRoot() { return(m_dirRoots.at(0)); }
	void addDirRoot(const QString& path);

	const QList<QString>	getOpenEditors() { return(m_openEditors); }
	const QList<QVariant>	getEditorPositions() { return(m_editorPositions); }
	const QList<QVariant>	getEditorScrolls() { return(m_editorScrolls); }
	int						getActiveEditorIndex();

	void			setResourceFilterType(FilterFileStyle style) { m_resourceFilterType = style; }
	FilterFileStyle	getResourceFilterType() { return(m_resourceFilterType); }

	void			setResourceFilters(QStringList newFilters);
	QStringList&	getResourceFilters() { return(m_resourceFilters); }

	void			setPovrayConsoleFilters(int filters) { m_povrayConsoleFilters = filters; }
	int				getPovrayConsoleFilters() { return(m_povrayConsoleFilters); }

	int	getRenderRefreshDelay() const { return(m_renderRefreshDelay); }
	void setRenderRefreshDelay(int milliDelay) { m_renderRefreshDelay = milliDelay; }

	void			setFileOpenLocation(const QString& path) { m_fileOpenLocation = path; }
	const QString&	getFileOpenLocation() { return(m_fileOpenLocation); }

	const QRect&	getColormapRect() { return(m_colormapRect); }
	void			setColormapRect(const QRect& rect) { m_colormapRect = rect; }

	const QRect&	getColorRect() { return(m_colorRect); }
	void			setColorRect(const QRect& rect) { m_colorRect = rect; }

	const QRect&	getColorModelessRect() { return(m_colorModelessRect); }
	void			setColorModelessRect(const QRect& rect) { m_colorModelessRect = rect; }

	const QRect&	getPreferencesRect() { return(m_prefsRect); }
	void			setPreferencesRect(const QRect& rect) { m_prefsRect = rect; }

	const QRect&	getAboutRect() { return(m_aboutRect); }
	void			setAboutRect(const QRect& rect) { m_aboutRect = rect; }

private:
	void			updateEditorPositions();

	QString			m_filename;				// The absolute path to this workspace file on disk
	QList<QString>	m_dirRoots;				// list of directories in the workspace
	QString			m_fileOpenLocation;		// when the user uses "File Open", this is the directory.
	QStringList		m_resourceFilters;
	FilterFileStyle	m_resourceFilterType;
	int				m_povrayConsoleFilters;
	int				m_renderRefreshDelay;

	// these are only valid immediately after load, so the editors can be opened where they belong
	QList<QString>	m_openEditors;			// list of open editors
	int				m_activeEditorIndex;	// which editor is open
	QList<QVariant>	m_editorPositions;		// ints.
	QList<QVariant>	m_editorScrolls;		// scroll position goes

	QRect			m_colormapRect;
	QRect			m_colorRect;
	QRect			m_colorModelessRect;
	QRect			m_prefsRect;
	QRect			m_aboutRect;
	MainWindow*		m_mainWindow;
};

//inline void Workspace::setActiveEditorIndex(int which) { m_activeEditorIndex = which; }
inline int  Workspace::getActiveEditorIndex() { return(m_activeEditorIndex); }

#endif // _WORKSPACE_H_
