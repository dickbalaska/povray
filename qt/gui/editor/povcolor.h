/******************************************************************************
 * povcolor.h - Manage a POV-Ray 5 term color vector
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
#ifndef _POVCOLOR_H_
#define _POVCOLOR_H_

#include <QColor>
#include <QObject>

class ColorEditor;
class PovColormap;

enum ColorType {
	Type_RGB,
	Type_RGBF,
	Type_RGBT,
	Type_RGBFT
};
extern QString colorTerm(double d);

class PovColor : public QColor
{
	friend class ColorEditor;
	friend class PovColormap;
public:
	PovColor();
	PovColor(const PovColor &other);
	PovColor& operator =(const PovColor& other);
	static void init();
	static QStringList colorTypes;

	double	filterF() const { return(m_filter); }
	double	transmitF() const { return(m_transmit); }
	void	setFilter(int f) { this->m_filter = (double)f/255.0; }
	void	setTransmit(int t) { this->m_transmit = (double)t/255.0; }
	void	setFilterF(double d) { this->m_filter = d; }
	void	setTransmitF(double d) { this->m_transmit = d; }

	void	parse(const QString& token, const QString& data);	// separate "rgb" and "<0,0,0>" data may or may not include brackets
	void	parse(const QString& data);							// "rgb<0,0,0>"
	QColor	toQColor() const;
	QString toString(int typeWidth = 0) const;

private:
	ColorType	m_colorType;
	double		m_filter;
	double		m_transmit;
};

class PovColormapEntry
{
	friend class PovColormap;
	friend class PovColormapEntryList;
	friend class CMWidget;

public:
	PovColormapEntry();
	PovColormapEntry(const PovColormapEntry& other);
	PovColormapEntry& operator=(const PovColormapEntry& other);
	//~ColormapEntry() {}
	QString		toString(int widest = 0, int widestType = 0);
private:
	PovColor	m_color;
	double		m_offset;
};

class PovColormapEntryList : public QList<PovColormapEntry*>
{
public:
	void addEntry(PovColormapEntry* entry);
};

class PovColormap
{
	friend class CMWidget;
public:
	PovColormap() {}
	PovColormap(const PovColormap& other);
	PovColormap& operator=(const PovColormap& other);

	void	parse(const QString& token, const QString& data);	// separate "color_map" and "{[0.0 color rgb<0,0,0>]}" data may or may not include brackets
	void	parse(const QString& text);

	QString toString() const;

	void	paint(QPixmap* target, bool horizontal);


private:
	PovColormapEntryList	m_map;

	friend QDebug& operator<<(QDebug&, const PovColormap&);
	//friend Q_GUI_EXPORT QDataStream& operator<<(QDataStream&, const PovColormap&);

};
QDebug& operator<<(QDebug&, const PovColormap&);
//Q_GUI_EXPORT QDataStream& operator<<(QDataStream&, const PovColormap&);

#endif // _POVCOLOR_H_
