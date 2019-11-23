/******************************************************************************
 * colormapeditor.h - Edit POV-Ray colormaps
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
#ifndef COLORMAPEDITOR_H_
#define COLORMAPEDITOR_H_

#include <QLabel>
#include <QDialog>
#include "povcolor.h"

class ColormapEditor;
class ColorEditor;
class MainWindow;

class CMWidget : public QLabel
{
	Q_OBJECT

	friend class ColormapEditor;
public:
	CMWidget(ColormapEditor* parent, PovColormap* colormap);

signals:
	void		editColor(const PovColor& color);

public slots:
	void		editedColorChanged(const PovColor& color);

protected:
	virtual QSize	sizeHint() const;
	virtual void 	resizeEvent(QResizeEvent*);
	virtual void 	paintEvent(QPaintEvent*);
	virtual void	contextMenuEvent(QContextMenuEvent* event);
	virtual void	mouseMoveEvent(QMouseEvent* event);
	virtual void	mousePressEvent(QMouseEvent* event);
	virtual void	mouseReleaseEvent(QMouseEvent* event);

private slots:
	void		deleteEntry();
	void		insertEntry();

private:
	void				calculateWidths();
	QString				getColormapColor(const PovColor& color);
	PovColormapEntry*	getColormapEntryAt(int y);
	double				getColormapOffsetAt(int y);
	PovColormap*		m_colormap;
	QFont				m_font;
	int					m_nominalLeftWidth;
	int					m_nominalRightWidth;
	int					m_leftWidth;
	int					m_centerWidth;
	int					m_rightWidth;
	int					m_fontHOfs;
	PovColormapEntry*	m_draggingEntry;
	PovColormapEntry*	m_selectedCme;
	double				m_newEntryOffset;
};

///////////////////////////////////////////////////////////////////////////////
class ColormapEditor : public QDialog
{
	Q_OBJECT
public:
	explicit ColormapEditor(QWidget *parent, MainWindow* mainWindow, const PovColormap& colormap);
	virtual ~ColormapEditor();

	const PovColormap& colormap() const { return(m_colormap); }

signals:

public slots:
	void		onEditColor(const PovColor& color);
	void		onEditColorClosed();

private:
	CMWidget*		m_cmwidget;
	PovColormap		m_colormap;
	ColorEditor*	m_colorEditor;
	MainWindow*		m_mainWindow;
};


#endif // COLORMAPEDITOR_H_
