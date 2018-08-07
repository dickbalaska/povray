/******************************************************************************
 * renderdock.h - The dockwidget that displays the graphic output of povray
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
 *
 * Display the Render output. Contains 4 classes:
 * RenderWidget - Manage the render output
 * RenderLabel - Display the render output
 * RenderDock - Container for RenderWidget when docked
 * RenderWindow - Container for RenderWidget when undocked
 *
 * Qt construction is to create the RenderDock. Then when it restores the
 * window positions, figure out whether it's really a dock or a window.
 *
 * ____________________________________________________________________________
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

#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QDockWidget>
#include <QLabel>
#include <QPicture>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include "../mainwindow.h"

class RenderDock;
class RenderLabel;
class RenderWindow;
class RenderWidget;

enum
{
	WSG_INIT = 0,
	WSG_DRAW_PIXEL_BLOCK,
	WSG_DRAW_FILLED_RECT,
	WSG_DRAW_PIXEL
};

class RenderLabel : public QLabel
{
	friend class RenderWidget;
	Q_OBJECT
public:
	RenderLabel(RenderWidget* parent);

protected slots:
	void	onResizeNatural();
	void	onSetRefreshDelay();

protected:
	virtual void 	resizeEvent(QResizeEvent *event);
	virtual void 	paintEvent(QPaintEvent *event);
	virtual void	contextMenuEvent(QContextMenuEvent* event);

private:
	RenderWidget*	m_renderWidget;
	QPixmap*	povrayPixmap;
	QPicture*	picture;
	bool		m_debug;
};

class RenderWidget : public QWidget
{
	friend class RenderLabel;
	Q_OBJECT
public:
	explicit RenderWidget(RenderDock* parent, MainWindow* mainWindow);
	bool	isFloating();
	bool	isDockable();
	bool	isDocked();
	RenderWindow*	getRenderWindow();

signals:

public slots:
	void	binaryMessageReceived(const QByteArray& data);
	void	timerTicked();
	void	onDisableDockable();
	void	onEnableDockable();

protected:
	virtual void 	resizeEvent(QResizeEvent *event);

private:
	void		doDrawThrottle();
	MainWindow*		m_mainWindow;
	RenderWindow*	m_renderWindow;
	RenderLabel		m_renderLabel;
	QPixmap			m_labelPixmap;
	QPicture		m_labelPicture;
	QPixmap			povrayPixmap;
	QPainter		painter;
	QTimer			m_repaintThrottle;
	bool			m_morePaintEvents;
	int				m_refreshDelayThrottle;
	bool			m_dockable;
	bool			m_debug;
};

inline bool RenderWidget::isDockable() { return(m_dockable); }
inline RenderWindow* RenderWidget::getRenderWindow() { return(m_renderWindow); }


class RenderWindow : public QWidget
{
	Q_OBJECT
public:
	explicit RenderWindow();
	void	setRenderWidget(RenderWidget* renderWidget) { m_renderWidget = renderWidget; }

protected:
	virtual void 	resizeEvent(QResizeEvent *event);

private:
	RenderWidget*	m_renderWidget;
};

class RenderDock : public QDockWidget
{
	Q_OBJECT

	friend class RenderLabel;

public:
	explicit RenderDock(MainWindow* parent = Q_NULLPTR, RenderWidget* renderWidget = nullptr, Qt::WindowFlags flags = nullptr);
	RenderWidget*	getRenderWidget() { return(m_renderWidget); }

signals:

public slots:

private:

	RenderWidget*	m_renderWidget;
	MainWindow*		m_mainWindow;
};

#endif // RENDERWINDOW_H
