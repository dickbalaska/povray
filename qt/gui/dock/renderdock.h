/******************************************************************************
 * renderdock.h - The dockwidget that displays the graphic output of povray
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

#ifndef _RENDERWINDOW_H_
#define _RENDERWINDOW_H_

#include <QDockWidget>
#include <QLabel>
#include <QPicture>
#include <QPainter>
#include <QPixmap>
#include <QTimer>
#include "../mainwindow.h"

class RenderDock;

enum
{
	WSG_INIT = 0,
	WSG_DRAW_PIXEL_BLOCK
};

class RenderLabel : public QLabel
{
	friend class RenderDock;
	Q_OBJECT
public:
	RenderLabel(RenderDock* parent);

protected slots:
	void	onResizeNatural();
	void	onSetRefreshDelay();

protected:
	virtual void 	resizeEvent(QResizeEvent *event);
	virtual void 	paintEvent(QPaintEvent *event);
	virtual void	contextMenuEvent(QContextMenuEvent* event);

private:
	RenderDock*	m_renderDock;
	QPixmap*	povrayPixmap;
	QPicture*	picture;
	bool		m_debug;
};

class RenderDock : public QDockWidget
{
	Q_OBJECT

	friend class RenderLabel;

public:
	explicit RenderDock(MainWindow* parent = Q_NULLPTR, Qt::WindowFlags flags = 0);

signals:

public slots:
	void	binaryMessageReceived(const QByteArray& data);
	void	timerTicked();

private:
	MainWindow* mainWindow;
	RenderLabel	m_renderLabel;
	QPixmap		m_labelPixmap;
	QPicture	m_labelPicture;
	QPixmap		povrayPixmap;
	QPainter	painter;
	QTimer		m_repaintThrottle;
	bool		m_morePaintEvents;
	int			m_refreshDelayThrottle;
	bool		m_debug;
};

#endif // _RENDERWINDOW_H_
