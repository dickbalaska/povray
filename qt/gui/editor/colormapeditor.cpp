/******************************************************************************
 * colormapeditor.cpp - Edit POV-Ray colormaps
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

#include "workspace.h"
#include "mainwindow.h"
#include "coloreditor.h"
#include "colormapeditor.h"

QColor backgroundColor(128, 128, 128);
QColor selectedColor(192,192,192);

ColormapEditor::ColormapEditor(QWidget *parent, MainWindow* mainWindow, const PovColormap& colormap)
	: QDialog(parent),
	  m_colormap(colormap),
	  m_mainWindow(mainWindow)
{
	m_colorEditor = NULL;
	setWindowTitle(tr("Colormap Editor"));
	QVBoxLayout* mainLayout = new QVBoxLayout();
	m_cmwidget = new CMWidget(this, &m_colormap);
	mainLayout->addWidget(m_cmwidget, 1);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										 | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	mainLayout->addWidget(buttonBox);
	this->setLayout(mainLayout);

	connect(m_cmwidget, SIGNAL(editColor(PovColor)), this, SLOT(onEditColor(PovColor)));
	Workspace* ws = m_mainWindow->getWorkspace();
	if (!ws->getColormapRect().topLeft().isNull()) {
		this->move(ws->getColormapRect().topLeft());
		this->resize(ws->getColormapRect().size());
	}
}

ColormapEditor::~ColormapEditor()
{
	Workspace* ws = m_mainWindow->getWorkspace();
	ws->setColormapRect(QRect(pos(), size()));
	qDebug() << "Updated ColormapEditor position";
}

void ColormapEditor::onEditColor(const PovColor &color)
{
	if (!m_colorEditor) {
		m_colorEditor = new ColorEditor(this, m_mainWindow, color, true);
		m_colorEditor->show();
		Workspace* ws = m_mainWindow->getWorkspace();
		if (!ws->getColorModelessRect().topLeft().isNull()) {
			m_colorEditor->move(ws->getColorModelessRect().topLeft());
			m_colorEditor->resize(ws->getColorModelessRect().size());
		} else {
			m_colorEditor->move(pos().x()+width()+10, this->pos().y());
		}
		connect(m_colorEditor, SIGNAL(colorChanged(PovColor)), m_cmwidget, SLOT(editedColorChanged(PovColor)));
		connect(m_colorEditor, SIGNAL(closeSignal()), this, SLOT(onEditColorClosed()));
	}
	else
		m_colorEditor->editColor(color);
}
void ColormapEditor::onEditColorClosed()
{
	qDebug() << "ColormapEditor::onEditColorClosed";
	m_colorEditor = NULL;
}
///////////////////////////////////////////////////////////////////////////////
CMWidget::CMWidget(ColormapEditor* parent, PovColormap* colormap)
	: QLabel(parent)
{
	m_colormap = colormap;
	m_draggingEntry = NULL;
	setMinimumWidth(250);
	setMinimumHeight(200);
//	m_font.setFamily("Courier");
	m_font.setFamily("Source Code Pro");
	m_font.setFixedPitch(true);
	m_font.setPointSize(9);
	QFontMetrics fm(m_font);
	m_nominalLeftWidth = fm.width("0000-");
	m_nominalRightWidth = fm.width("r=0000 g=0000 b=0000 f=0000 t=0000");
	m_fontHOfs = fm.height()/2;
	calculateWidths();
	qDebug() << "size" << this->size() << m_leftWidth << m_centerWidth << m_rightWidth;
}

QSize	CMWidget::sizeHint() const
{
	return(QSize(370, 300));
}

void CMWidget::editedColorChanged(const PovColor& color)
{
	if (m_selectedCme) {
		m_selectedCme->m_color = color;
		repaint();
	}
}

void CMWidget::calculateWidths()
{
	int minimumCenterWidth=30;
	int w = this->width();
	m_leftWidth = m_nominalLeftWidth;
	m_centerWidth = minimumCenterWidth;
	m_rightWidth = m_nominalRightWidth;
	if (w > m_nominalLeftWidth + m_nominalRightWidth + minimumCenterWidth) {
		int r = (w - (m_nominalLeftWidth + m_nominalRightWidth + minimumCenterWidth))/3;
		m_leftWidth += r;
		m_centerWidth += r;
		m_rightWidth += r;
		if (r*3 < w)
			m_centerWidth++;
		return;
	}
	int r = ((m_nominalLeftWidth + m_nominalRightWidth + minimumCenterWidth)-w)/3;
	if (r < 20) {
		m_leftWidth -= r;
		m_centerWidth -= r;
		m_rightWidth -= r;
		return;
	}
	r = (r*3-20)/2;
	m_leftWidth -= r;
	m_centerWidth -=20;
	m_rightWidth -= r;
}

void CMWidget::resizeEvent(QResizeEvent*)
{
	calculateWidths();
	qDebug() << "resize" << this->size() << m_leftWidth << m_centerWidth << m_rightWidth;
}

#define interpolate(start, end, offset) ((end-start)*offset+start)

void CMWidget::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	QFontMetrics fm(m_font);
	p.setPen(Qt::black);
	p.setFont(m_font);
	p.fillRect(0,0,width(), height(), backgroundColor);
	QPixmap pm(m_centerWidth, height()-m_fontHOfs*2);
	m_colormap->paint(&pm, false);
	int mar = m_fontHOfs;
	int mar2 = mar*2;
	p.drawPixmap(m_leftWidth, m_fontHOfs, pm);
	foreach(PovColormapEntry* cme, m_colormap->m_map) {

		double row = height()-mar-interpolate(0.0, (double)(height()-(double)mar2), cme->m_offset);
		int tr = row+mar-1;	// text row
		int lr = row;		// line row
		QString s = QString::number(cme->m_offset, 'f', 2);
//		if (s.length() == 1)
//			s.append(".00");
//		if (s.length() == 3)
//			s.append("0");
		if (cme == m_selectedCme) {
			int r = fm.width(s);
			p.fillRect(4, tr+2, r+1, -(fm.height()+1), selectedColor);
		}

		p.drawText(5, tr, s);
		if (m_leftWidth > m_nominalLeftWidth) {
			p.drawLine(m_nominalLeftWidth, lr, m_leftWidth-1, lr);
		}
		s = getColormapColor(cme->m_color);
		if (cme == m_selectedCme) {
			int r = fm.width(s);
			p.fillRect(m_leftWidth+m_centerWidth+4, tr+2, r+1, -(fm.height()+1), selectedColor);
		}
		p.drawText(m_leftWidth+m_centerWidth+5,tr, s);
	}
}

QString CMWidget::getColormapColor(const PovColor& color)
{
#define _number(C, V)	s.append(C); s.append(':'); \
	if (V == 1.0) s.append("1.00"); else s.append(QString::number(V, 'f', 3).right(4));

	QString s;
	_number ('r', color.redF());
	s.append(' ');
	_number ('g', color.greenF());
	s.append(' ');
	_number ('b', color.blueF());
	s.append(' ');
	_number ('f', color.filterF());
	s.append(' ');
	_number ('t', color.transmitF());
	return(s);
}
void CMWidget::contextMenuEvent(QContextMenuEvent* event)
{
	qDebug() << "contextMenuEvent" << event->pos();
	if (event->pos().x() < m_leftWidth) {
		QMenu menu(this);
		PovColormapEntry* cme = getColormapEntryAt(event->pos().y());
		if (cme) {
			m_selectedCme = cme;
			QString s = tr("Remove %1").arg(QString::number(cme->m_offset, 'f', 2));
			menu.addAction(s, this, SLOT(deleteEntry()));
		} else {
			double d = getColormapOffsetAt(event->pos().y());
			QString s = tr("Insert %1").arg(QString::number(d, 'f', 2));
			menu.addAction(s, this, SLOT(insertEntry()));
			m_newEntryOffset = d;
		}
		menu.exec(event->globalPos());
	}
}
void CMWidget::mouseMoveEvent(QMouseEvent* event)
{
	if (!m_draggingEntry)
		return;
	qDebug() << "mouseMoveEvent" << event->pos();
	if (m_draggingEntry) {
		double d = getColormapOffsetAt(event->pos().y());
		m_draggingEntry->m_offset = d;
		repaint();
	}
}
void CMWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() & Qt::RightButton)
		return;
	qDebug() << "mousePressEvent" << event->pos();
	PovColormapEntry* cme = getColormapEntryAt(event->pos().y());
	m_selectedCme = cme;
	if (event->x() < m_leftWidth) {
		m_draggingEntry = cme;
	} else if (event->x() >= m_leftWidth+m_centerWidth) {
		if (cme)
			emit(editColor(cme->m_color));
	}
	repaint();
}
void CMWidget::mouseReleaseEvent(QMouseEvent* event)
{
	qDebug() << "mouseReleaseEvent" << event->pos();
	m_draggingEntry = NULL;

}

PovColormapEntry* CMWidget::getColormapEntryAt(int y)
{
	int mar = m_fontHOfs;
	int mar2 = mar*2;
	foreach(PovColormapEntry* cme, m_colormap->m_map) {
		double row = height()-mar-interpolate(0.0, (double)(height()-(double)mar2), cme->m_offset);
		int tr = row+mar-1;	// text row
		if (tr > y && tr-m_fontHOfs*2 < y)
			return(cme);
	}
	return(NULL);
}

double CMWidget::getColormapOffsetAt(int y)
{
	int mar = m_fontHOfs;
	int start = height()-mar;
	int end = mar;
	y = y-mar;
	int dist = end-start;
	double d = 1.0+((double)y/(double)dist);
	if (d > 1.0) d = 1.0;
	if (d < 0.0) d = 0.0;
	return(d);
}

void CMWidget::deleteEntry()
{
	if (m_selectedCme) {
		m_colormap->m_map.removeOne(m_selectedCme);
		repaint();
	}
}
void CMWidget::insertEntry()
{
	PovColormapEntry* cme = new PovColormapEntry();
	cme->m_offset = m_newEntryOffset;
	m_colormap->m_map.addEntry(cme);
	repaint();
}
