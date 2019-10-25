/******************************************************************************
 * povcolor.cpp - Manage a POV-Ray 5 term color vector
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

#include <QPainter>
#include <QPixmap>
#include <QDataStream>
#include <QDebug>

#include "povcolor.h"

QStringList PovColor::colorTypes;

// static
void PovColor::init()
{
	colorTypes << "rgb" << "rgbf" << "rgbt" << "rgbft";
}

PovColor::PovColor()
	: QColor(Qt::black),
	  m_colorType(Type_RGB),
	  m_filter(0),
	  m_transmit(0) {}

PovColor::PovColor(const PovColor& other)
	: QColor(other),
	  m_colorType(other.m_colorType),
	  m_filter(other.m_filter),
	  m_transmit(other.m_transmit) {}

PovColor& PovColor::operator=(const PovColor& other)
{
	QColor::operator=(other);
	this->m_colorType = other.m_colorType;
	this->m_filter = other.m_filter;
	this->m_transmit = other.m_transmit;
	return(*this);
}

QString colorTerm(double d)
{
	//double d = (double)i/255.0;
	return(QString::number(d, 'f', 4));
}

QString PovColor::toString(int typeWidth) const
{
	QString s = colorTypes.at(m_colorType);
	if (typeWidth)
		for (int i = s.length(); i<typeWidth; i++)
			s.append(' ');
	s.append(QString("<%1, %2, %3").arg(colorTerm(this->redF()))
								   .arg(colorTerm(this->greenF()))
								   .arg(colorTerm(this->blueF())));
	if (m_colorType == Type_RGBF || m_colorType == Type_RGBFT)
		s.append(QString(", %2").arg(colorTerm(this->m_filter)));
	if (m_colorType == Type_RGBT || m_colorType == Type_RGBFT)
		s.append(QString(", %2").arg(colorTerm(this->m_transmit)));
	s.append('>');
	return(s);
}

QColor PovColor::toQColor() const
{
	QColor c(*this);
	if (m_colorType == Type_RGBF || m_colorType == Type_RGBFT) {
		c.setRedF(c.redF()*1.0-m_filter);
		c.setBlueF(c.blueF()*1.0-m_filter);
		c.setGreenF(c.greenF()*1.0-m_filter);
	}
	double alpha = 1.0;
	if (m_colorType == Type_RGBT || m_colorType == Type_RGBFT)
		alpha -= m_transmit;
	c.setAlphaF(alpha);
	return(c);
}

void PovColor::parse(const QString& token, const QString& data)
{
	qDebug() << "token:" << token << "data:" <<data;
	QString z = data.trimmed();
	if (z.startsWith('<'))
		z = z.right(z.length()-1);
	if (z.endsWith('>'))
		z = z.left(z.length()-1);
	QStringList sl = z.split(',');
	QList<double> dlist;
	int i=0;
	foreach(const QString& s, colorTypes) {
		if (token == s) {
			this->m_colorType = (ColorType)i;
			break;
		}
		i++;
	}
	foreach(QString s, sl) {
		dlist.append(s.toDouble());
	}
	double r=0.0, g=0.0, b=0.0, f=0.0, t=0.0;
	if (dlist.length()>0)
		r = dlist[0];
	if (dlist.length()>1)
		g = dlist[1];
	if (dlist.length()>2)
		b = dlist[2];
	if (dlist.length() == 4) {
		if (m_colorType == Type_RGBT)
			t = dlist[3];
		else
			f = dlist[3];
	}
	if (dlist.length() == 5)
		t = dlist[4];

	qDebug() << "parseColor:" << colorTypes[m_colorType] << "r:" << r << "g:" << g << "b:" << b << "f:" << f << "t:" << t;
	this->setRgb(r*255,g*255,b*255);
	this->m_filter = f;
	this->m_transmit = t;

}
void PovColor::parse(const QString& data)
{
	int i, j;
	for (i = 0; i<data.length(); i++) {
		if (!data[i].isSpace())
			break;
	}
	for (j=i; j<data.length(); j++)
		if (!data[j].isLetter())
			break;
	QString token = data.mid(i, j-i);
	parse(token, data.mid(j));
}

///////////////////////////////////////////////////////////////////////////////
PovColormapEntry::PovColormapEntry() {}
PovColormapEntry::PovColormapEntry(const PovColormapEntry& other)
{
	this->m_color = other.m_color;
	this->m_offset = other.m_offset;
}
PovColormapEntry& PovColormapEntry::operator=(const PovColormapEntry& other)
{
	this->m_color = other.m_color;
	this->m_offset = other.m_offset;
	return(*this);
}

QString PovColormapEntry::toString(int widest, int widestType)
{
	QString s = "[";
	s.append(QString::number(this->m_offset));
	if (widest)
		for (int i=s.length()-1; i<widest+1; i++)
			s.append(' ');
	else
		s.append(' ');
	s.append("color ");
	s.append(m_color.toString(widestType));
	s.append(']');
//	QString s = QString("[%1 color %2]").arg(QString::number(this->m_offset)).arg(m_color.toString());
	return(s);
}

///////////////////////////////////////////////////////////////////////////////
QDebug& operator<<(QDebug& ds, const PovColormap& cm)
{
	ds << cm.m_map.size();
	foreach (PovColormapEntry* cme, cm.m_map) {
		ds << cme->toString();
	}
	return(ds);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief PovColormapEntryList::addEntry
/// \param entry
/// Entries stored in the list are ordered
void PovColormapEntryList::addEntry(PovColormapEntry* entry)
{
	if (this->isEmpty()) {
		this->append(entry);
		return;
	}
	for (int i=0; i<this->size(); i++) {
		if (entry->m_offset < this->at(i)->m_offset) {
			this->insert(i, entry);
			return;
		}
	}
	this->append(entry);
}

///////////////////////////////////////////////////////////////////////////////
PovColormap::PovColormap(const PovColormap& other)
{
	m_map.clear();
	foreach(PovColormapEntry* cme, other.m_map) {
		PovColormapEntry* p(cme);
		m_map.addEntry(p);
	}
}
PovColormap& PovColormap::operator=(const PovColormap& other)
{
	m_map.clear();
	foreach(PovColormapEntry* cme, other.m_map) {
		PovColormapEntry* p(cme);
		m_map.addEntry(p);
	}
	return(*this);
}

void PovColormap::parse(const QString&, const QString &data)
{
	parse(data);
}

void PovColormap::parse(const QString& text)
{
	qDebug() << "PovColormap::parse:" << text;
	m_map.clear();
	int index = 0;
	// bleh. Rewrite this with regexp
	while (true) {
		int leftbracket = text.indexOf('[', index);
		if (leftbracket == -1)
			break;
		leftbracket++;
		int rightbracket = text.indexOf(']', leftbracket);
		if (rightbracket == -1)
			break;
		int i;
		for (i=leftbracket; i<rightbracket; i++) {
			if (!text[i].isSpace())
				break;
		}
		if (i == rightbracket)
			break;
		for (; i<rightbracket; i++)
			if (text[i].isSpace())
				break;
		if (i == rightbracket)
			break;

		double d = text.mid(leftbracket, i-leftbracket).toDouble();
		i++;
		for(; i<rightbracket; i++) {	// skip over the space before the color keyword
			if (!text[i].isSpace())
				break;
		}
		for(; i<rightbracket; i++) {	// skip over the color keyword
			if (text[i].isSpace())
				break;
		}
		if (i == rightbracket)
			break;
		i++;
		for (; i<rightbracket; i++)
			if (!text[i].isSpace())
				break;
		if (i == rightbracket)
			break;
		// now we are pointing at the "rgb"
		int j = i;
		PovColor pc;
		QString colorString;
		for (; i<rightbracket; i++) {
			if (text[i] == '>') {
				colorString = text.mid(j, i-j+1);
				break;
			}
		}
		//qDebug() << "colorString" << colorString;
		if (!colorString.isEmpty()) {
			pc.parse(colorString);
		}
		PovColormapEntry* cme = new PovColormapEntry();
		cme->m_offset = d;
		cme->m_color = pc;
		m_map.addEntry(cme);
		index = rightbracket+1;
	}
	qDebug() << "m_map" << *this;
}

QString PovColormap::toString() const
{
	QString s = "color_map{";
	bool first = true;
	int widest = 0;
	int	widestType = 0;
	foreach(PovColormapEntry* cme, m_map) {
		int w = QString::number(cme->m_offset).length();
		if (w > widest)
			widest = w;
		if (PovColor::colorTypes.at(cme->m_color.m_colorType).length() > widestType)
			widestType = PovColor::colorTypes.at(cme->m_color.m_colorType).length();
	}

	foreach(PovColormapEntry* cme, m_map) {
		QString t = cme->toString(widest, widestType);
		if (!first)
			s.append("          ");
		s.append(t);
		s.append('\n');
		first = false;
	}
	s.append("}");
	return(s);
}

#define interpolate(start, end, offset) ((end-start)*offset+start)

void PovColormap::paint(QPixmap* target, bool horizontal)
{
	QPainter p(target);
	int size = 10;
	int w = target->width()/size;
	int h = target->height()/size;
	for (int x=0; x<=w; x++) {
		for (int y=0; y<=h; y++) {
			if ((x+y)&1)
				p.fillRect(x*size, y*size, 10, 10, Qt::black);
			else
				p.fillRect(x*size, y*size, 10, 10, Qt::white);
		}
	}
	if (m_map.isEmpty())
		return;
	w = target->width();
	h = target->height();
	int end = h;
	if (horizontal)
		end = w;
	for (int i=0; i<end; i++) {
		double cmoffset = (double)i/(double)end;
		PovColormapEntry* before = nullptr;
		PovColormapEntry* after = nullptr;
		foreach(PovColormapEntry* cme, m_map) {
			if (before == nullptr || cme->m_offset <= cmoffset)
				before = cme;
			if (cme->m_offset >= cmoffset && after == nullptr)
				after = cme;
		}
		PovColor c1 = before->m_color;
		if (after && before != after) {
			PovColor c2 = after->m_color;
			double ioffset = (cmoffset-before->m_offset) / (after->m_offset - before->m_offset);
			c1.setRedF(interpolate(c1.redF(), c2.redF(), ioffset));
			c1.setGreenF(interpolate(c1.greenF(), c2.greenF(), ioffset));
			c1.setBlueF(interpolate(c1.blueF(), c2.blueF(), ioffset));
			c1.setFilterF(interpolate(c1.filterF(), c2.filterF(), ioffset));
			c1.setTransmitF(interpolate(c1.transmitF(), c2.transmitF(), ioffset));
			//qDebug() << "c1:" << i << QString::number(cmoffset, 'f', 3)
			//		 << QString::number(ioffset, 'f', 3)
			//		 << QString::number(c1.redF() , 'f', 3)<< QString::number(c1.greenF(), 'f', 3)
			//		 << QString::number(c1.blueF(), 'f', 3) << QString::number( c1.filterF(), 'f', 3)
			//		 << QString::number(c1.transmitF(), 'f', 3);
		}
		c1.m_colorType = Type_RGBFT;
		p.setPen(c1.toQColor());
		if (horizontal)
			p.drawLine(i, 0, i, h);
		else
			p.drawLine(0, h-i-1, w, h-i-1);
	}
}

