/******************************************************************************
 * coloreditor.cpp - Edit POV-Ray colors
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
#ifndef _COLOREDITOR_H_
#define _COLOREDITOR_H_

#include <QDialog>

#include "povcolor.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QRadioButton;
class QSlider;
QT_END_NAMESPACE

class ClickableLabel;
class MainWindow;

class ColorEditor : public QDialog
{
	Q_OBJECT
public:
	explicit ColorEditor(QWidget* parent, MainWindow* mainWindow, PovColor color, bool noOkCancel = false);
	virtual ~ColorEditor();

	const PovColor& color() const { return(m_color); }
signals:
	void		colorChanged(const PovColor& color);
	void		closeSignal();

public slots:
	void		labelClicked();
	void		typeChanged();
	void		sliderChanged();
	void		editColor(const PovColor& color);

protected:
	virtual void closeEvent(QCloseEvent* event);

private slots:

private:
	void		update(bool fromSlider = false);

//	QRadioButton*	m_typeRGB;
//	QRadioButton*	m_typeRGBF;
//	QRadioButton*	m_typeRGBT;
//	QRadioButton*	m_typeRGBFT;
	ClickableLabel*	m_paintLabel;
	QPixmap			m_paintPixmap;
	QPixmap			m_backgroundPixmap;
	PovColor		m_color;
	QLabel*			m_dataRed;
	QLabel*			m_dataGreen;
	QLabel*			m_dataBlue;
	QLabel*			m_lablFilter;
	QLabel*			m_dataFilter;
	QLabel*			m_lablTransmit;
	QLabel*			m_dataTransmit;

	QList<QRadioButton*>	m_typeList;
	QList<QSlider*>			m_sliderList;
	QList<QLabel*>			m_sliderLabelList;
	bool			m_inUpdate;

	MainWindow*		m_mainWindow;
	bool			m_modeless;
};
#endif // _COLOREDITOR_H_
