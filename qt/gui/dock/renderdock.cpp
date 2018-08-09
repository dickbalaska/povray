/******************************************************************************
 * renderdock.cpp - The dockwidget that displays the graphic output of povray
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
 * Primary communication is with the RenderWidget. Only window management
 * cares about the container.
 *
 * ____________________________________________________________________________
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
 *****************************************************************************/

#include <QtWidgets>
#include <QDebug>

#include "../mainwindow.h"
#include "dockman.h"
#include "workspace.h"
#include "renderdock.h"



RenderDock::RenderDock(MainWindow* parent, RenderWidget* renderWidget, Qt::WindowFlags flags)
	: QDockWidget(QString("Render View"), parent, flags),
	  m_mainWindow(parent)
{
	this->setObjectName("RenderDock");
	if (renderWidget)
		m_renderWidget = renderWidget;
	else
		m_renderWidget = new RenderWidget(this, m_mainWindow);
	this->setWidget(m_renderWidget);
}



///////////////////////////////////////////////////////////////////////////////
RenderLabel::RenderLabel(RenderWidget* parent)
	: QLabel(parent),
	  m_renderWidget(parent)
{}

void RenderLabel::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();
	if (m_debug)
		qDebug() << "RenderLabel::resizeEvent w:" << size.width() << "h:" << size.height();
	QSize psize = povrayPixmap->size();
	if (!psize.height())
		return;
	QSize targetSize;
	if (size.width() * psize.height() / psize.width() < size.height()) {
		targetSize.setWidth(size.width());
		targetSize.setHeight(size.width() * psize.height() / psize.width());
		if (m_debug)
			qDebug() << "target size: " << targetSize;
	} else {
		targetSize.setHeight(size.height());
		targetSize.setWidth(size.height() * psize.width() / psize.height());
	}
	picture->setBoundingRect(QRect(QPoint(), size));
	int xofs = (size.width() -targetSize.width()) / 2;
	int yofs = (targetSize.height()-size.height()) / 2;
	if (yofs < 0) yofs = -yofs;
	QPainter	painter(picture);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::gray);
	//painter.drawPixmap(xofs, yofs, povrayPixmap->scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	painter.drawPixmap(xofs, yofs, povrayPixmap->scaled(targetSize, Qt::KeepAspectRatio, Qt::FastTransformation));
}

void 	RenderLabel::paintEvent(QPaintEvent *event)
{
	QLabel::paintEvent(event);
	QSize size = this->size();
	if (m_debug)
		qDebug() << "RenderLabel::paintEvent w:" << size.width() << "h:" << size.height();
	QSize psize = povrayPixmap->size();
	if (!psize.height())
		return;
	QSize targetSize;
	if (size.width() * psize.height() / psize.width() < size.height()) {
		targetSize.setWidth(size.width());
		targetSize.setHeight(size.width() * psize.height() / psize.width());
		//if (m_debug)
		//	qDebug() << "target size: " << targetSize;
	} else {
		targetSize.setHeight(size.height());
		targetSize.setWidth(size.height() * psize.width() / psize.height());
	}
	picture->setBoundingRect(QRect(QPoint(), size));
	int xofs = (size.width()-targetSize.width()) / 2;
	int yofs = (targetSize.height()-size.height()) / 2;
	if (yofs < 0) yofs = -yofs;
	QPainter	painter(picture);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::gray);
	painter.drawPixmap(xofs, yofs, povrayPixmap->scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void RenderLabel::contextMenuEvent(QContextMenuEvent* event)
{
	qDebug() << "RenderLabel::contextMenuEvent" << event->pos();
	QMenu menu(this);
	if (m_renderWidget->isFloating()) {
		QString t;
		QSize pms = m_renderWidget->povrayPixmap.size();
		t = tr("Resize to %1x%2").arg(QString::number(pms.width()), QString::number(pms.height()));
		menu.addAction(t, this, SLOT(onResizeNatural()));
		if (m_renderWidget->m_dockable) {
			QAction* action = menu.addAction(tr("Always on top"), m_renderWidget, SLOT(onDisableDockable()));
			action->setCheckable(true);
			action->setChecked(true);
		} else {
			menu.addAction(tr("Enable docking"), m_renderWidget, SLOT(onEnableDockable()));
		}
	}
	menu.addAction(tr("Set refresh delay"), this, SLOT(onSetRefreshDelay()));
	menu.exec(event->globalPos());
}

void RenderLabel::onResizeNatural()
{
	QSize pms = m_renderWidget->povrayPixmap.size();
	QSize pps = m_renderWidget->m_renderLabel.size();
	QSize parentSize = m_renderWidget->size();
	QSize diff = parentSize - pps;
	pms += diff;
	m_renderWidget->resize(pms);
}

void RenderLabel::onSetRefreshDelay()
{
	QDialog d(this);
	d.setWindowTitle(tr("Adjust refresh delay"));
	QVBoxLayout* mainLayout = new QVBoxLayout(&d);
	QHBoxLayout* hLayout = new QHBoxLayout();
	mainLayout->addLayout(hLayout);
	hLayout->addWidget(new QLabel(tr("Minimum number of milliseconds before drawing again")));
	QSpinBox* sb = new QSpinBox(this);
	sb->setRange(10,9999);
	sb->setValue(m_renderWidget->m_mainWindow->getWorkspace()->getRenderRefreshDelay());
	hLayout->addWidget(sb);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										 | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, &d, &QDialog::reject);
	mainLayout->addWidget(buttonBox);
	int ret = d.exec();
	if (ret == QDialog::Accepted) {
		m_renderWidget->m_mainWindow->getWorkspace()->setRenderRefreshDelay(sb->value());
		m_renderWidget->m_refreshDelayThrottle = sb->value();
	}
}

///////////////////////////////////////////////////////////////////////////////
RenderWidget::RenderWidget(RenderDock* parent, MainWindow* mainWindow)
	: QWidget(parent),
	  m_mainWindow(mainWindow),
	  m_renderWindow(nullptr),
	  m_renderLabel(this),
	  m_morePaintEvents(false),
	  m_refreshDelayThrottle(500),
	  m_dockable(true)

{
	m_debug = true;
	povrayPixmap = *mainPixmap;
	m_renderLabel.setScaledContents(true);
	m_renderLabel.setPicture(m_labelPicture);
	m_renderLabel.picture = &m_labelPicture;
	m_renderLabel.povrayPixmap = &povrayPixmap;
	m_renderLabel.m_debug = m_debug;
	QVBoxLayout* vLayout = new QVBoxLayout();
	vLayout->addWidget(&m_renderLabel);
	this->setLayout(vLayout);
	//this->setWidget(&m_renderLabel);
	m_repaintThrottle.setSingleShot(true);
	connect(&m_repaintThrottle, SIGNAL(timeout()), this, SLOT(timerTicked()));
}

bool RenderWidget::isFloating()
{
	if (!m_dockable)
		return(true);
	RenderDock* rd = qobject_cast<RenderDock*>(parent());
	return(rd->isFloating());
}

void RenderWidget::binaryMessageReceived(const QByteArray& data)
{
	QDataStream ds(data);
	ds.setByteOrder(QDataStream::LittleEndian);
	quint32 opcode;
	ds >> opcode;
	if (opcode == WSG_INIT) {
		quint32 w, h;
		ds >> w >> h;
		if (m_debug)
			qDebug() << "Create display: w=" << w << "h=" << h;
		povrayPixmap = QPixmap(w,h);
		povrayPixmap.fill();
		m_renderLabel.resize(m_renderLabel.size() + QSize(1,1));
		m_renderLabel.resize(m_renderLabel.size() - QSize(1,1));
		//renderLabel.repaint();
	} else if (opcode == WSG_DRAW_PIXEL_BLOCK) {
		quint32 x1, y1, x2, y2;
		ds >> x1 >> y1 >> x2 >> y2;
		if (m_debug)
			qDebug() << "WSG_DRAW_PIXEL_BLOCK" << x1 << "/" << y1;
		QPainter	painter(&povrayPixmap);
		QColor color;
		quint8 r,g,b,a;
		for (unsigned int y = y1; y <= y2; y++) {
			for (unsigned int x = x1; x <= x2; x++) {
				ds >> r >> g >> b >> a;
				color.setRgb(r,g,b,a);
				painter.setPen(color);
				painter.drawPoint(x,y);
			}
		}
		doDrawThrottle();
	} else if (opcode == WSG_DRAW_FILLED_RECT) {
		quint32 x1, y1, x2, y2;
		ds >> x1 >> y1 >> x2 >> y2;
		if (m_debug)
			qDebug() << "WSG_DRAW_FILLED_RECT" << x1 << "/" << y1;
		QRect rect;
		rect.setLeft(x1);
		rect.setTop(y1);
		rect.setRight(x2);
		rect.setBottom(y2);
		QPainter	painter(&povrayPixmap);
		QColor color;
		quint8 r,g,b,a;
		ds >> r >> g >> b >> a;
		color.setRgb(r,g,b,a);
		painter.fillRect(rect, color);
		doDrawThrottle();

	} else if (opcode == WSG_DRAW_PIXEL) {
		quint32 x, y;
		ds >> x >> y;
		if (m_debug)
			qDebug() << "WSG_DRAW_PIXEL" << x << "/" << y;
		QPainter	painter(&povrayPixmap);
		QColor color;
		quint8 r,g,b,a;
		ds >> r >> g >> b >> a;
		color.setRgb(r,g,b,a);
		painter.setPen(color);
		painter.drawPoint(x,y);
		doDrawThrottle();

	} else {
		qWarning() << "RenderDock::binaryMessageReceived: bad opcode" << opcode;
	}
}

void RenderWidget::doDrawThrottle()
{
	if (m_repaintThrottle.isActive()) {
		m_morePaintEvents = true;
		if (m_debug)
			qDebug() << "queue repaint";
		return;
	} else {
		m_renderLabel.repaint();
		m_repaintThrottle.start(m_refreshDelayThrottle);
		m_morePaintEvents = false;
		if (m_debug)
			qDebug() << "start repaint timer";
	}

}
void RenderWidget::timerTicked()
{
	if (m_morePaintEvents) {
		if (m_debug)
			qDebug() << "fire repaint";
		m_renderLabel.repaint();
		m_morePaintEvents = false;
	} else {
		if (m_debug)
			qDebug() << "timer expired";
	}
}

void RenderWidget::onDisableDockable()
{
	qDebug() << "onDisableDockable";

	m_dockable = false;
	m_renderWindow = new RenderWindow();
	RenderDock* rd = qobject_cast<RenderDock*>(parent());
	QSize rdsize = rd->size();
	QPoint rdpos = rd->pos();
	m_renderWindow->setRenderWidget(this);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->setContentsMargins(0,0,0,0);
	layout->addWidget(this);
	m_renderWindow->setLayout(layout);
	m_renderWindow->move(rdpos);
	m_renderWindow->resize(rdsize);
	//this->setParent(m_renderWindow);
	m_renderWindow->show();
	rd->close();
}

void RenderWidget::onEnableDockable()
{
	qDebug() << "onEnableDocking";
	m_dockable = true;
	RenderDock* rd = m_mainWindow->getDockMan()->openRenderDock(this);
	rd->setFloating(true);
//	this->setParent(rd);
	QPoint rwpos = m_renderWindow->pos();
	QSize rwsize = m_renderWindow->size();
	rd->move(rwpos);
	rd->resize(rwsize);
	m_renderWindow->close();
	m_renderWindow = nullptr;

}

///////////////////////////////////////////////////////////////////////////////
RenderWindow::RenderWindow()
	: QWidget(Q_NULLPTR, Qt::Window)
{
	this->setObjectName("Render Window");
}

void RenderWindow::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();
	qDebug() << "RenderWindow::resizeEvent w:" << size.width() << "h:" << size.height();
	QSize rwsize = this->size();
	qDebug() << "rwsize" << rwsize;
	qDebug() << "widget" << m_renderWidget->size();
	QRect r = this->geometry();
	QSize widgetSize = QSize(r.width(), r.height());
	m_renderWidget->resize(widgetSize);
}
