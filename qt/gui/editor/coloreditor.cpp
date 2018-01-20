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

#include <QtWidgets>

#include "mainwindow.h"
#include "workspace.h"
#include "coloreditor.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief The ClickableLabel class
///
class ClickableLabel : public QLabel
{
public:
	ClickableLabel(ColorEditor* parent) : QLabel(parent), m_parent(parent) {}
protected:
	virtual void mousePressEvent(QMouseEvent*);
private:
	ColorEditor* m_parent;
};
void ClickableLabel::mousePressEvent(QMouseEvent*)
{
	m_parent->labelClicked();
}

///////////////////////////////////////////////////////////////////////////////
/// \brief ColorEditor::ColorEditor
/// \param parent
/// \param color
/// \param type
///
ColorEditor::ColorEditor(QWidget* parent, MainWindow* mainWindow, PovColor color, bool noOkCancel)
	: QDialog(parent),
	  m_paintPixmap(100,100),
	  m_backgroundPixmap(100,100),
	  m_mainWindow(mainWindow),
	  m_modeless(noOkCancel)
{
	m_inUpdate = false;
	m_color = color;
	setWindowTitle(tr("Color Editor"));
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QGroupBox* typeGroup = new QGroupBox(tr("Type"));
	QVBoxLayout* gl = new QVBoxLayout();
	gl->setSpacing(0);
	gl->setMargin(5);
	foreach(const QString& s, PovColor::colorTypes) {
		QRadioButton* rb = new QRadioButton(s);
		m_typeList.append(rb);
		gl->addWidget(rb);
		connect(rb, SIGNAL(clicked(bool)), this, SLOT(typeChanged()));
	}
	gl->addStretch(1);
	typeGroup->setLayout(gl);
	mainLayout->addWidget(typeGroup);
	QHBoxLayout* pixmapLayout = new QHBoxLayout();
	m_paintLabel = new ClickableLabel(this);
	m_paintLabel->setPixmap(m_paintPixmap);
	pixmapLayout->addWidget(m_paintLabel);

	QVBoxLayout* labelOuterBox = new QVBoxLayout();
	QGridLayout* rgbftDataLayout = new QGridLayout();
	rgbftDataLayout->setSpacing(0);
	rgbftDataLayout->setMargin(5);
	QLabel* l = new QLabel(tr("Red:"));
	rgbftDataLayout->addWidget(l, 0, 0);
	m_dataRed = new QLabel("0.0");
	rgbftDataLayout->addWidget(m_dataRed, 0, 1);
	l = new QLabel(tr("Green:"));
	rgbftDataLayout->addWidget(l, 1, 0);
	m_dataGreen = new QLabel("0.0");
	rgbftDataLayout->addWidget(m_dataGreen, 1, 1);
	l = new QLabel(tr("Blue:"));
	rgbftDataLayout->addWidget(l, 2, 0);
	qDebug() << "margin" << l->margin();
	m_dataBlue = new QLabel("0.0");
	rgbftDataLayout->addWidget(m_dataBlue, 2, 1);
	m_lablFilter = new QLabel(tr("Filter:"));
	rgbftDataLayout->addWidget(m_lablFilter, 3, 0);
	m_dataFilter = new QLabel("0.0");
	rgbftDataLayout->addWidget(m_dataFilter, 3, 1);
	m_lablTransmit = new QLabel(tr("Transmit:"));
	rgbftDataLayout->addWidget(m_lablTransmit, 4, 0);
	m_dataTransmit = new QLabel("0.0");
	rgbftDataLayout->addWidget(m_dataTransmit, 4, 1);
	labelOuterBox->addLayout(rgbftDataLayout);
	labelOuterBox->addStretch(1);
	pixmapLayout->addLayout(labelOuterBox);

	mainLayout->addLayout(pixmapLayout);

	QStringList sl;
	sl << tr("R:") << tr("G:") << tr("B") << tr("F:") << tr("T:");

	QGridLayout* sliderLayout = new QGridLayout();
	int index = 0;
	foreach(const QString& s, sl) {
		QLabel* l = new QLabel(s);
		QSlider* slider = new QSlider(Qt::Horizontal);
		slider->setMinimum(0);
		slider->setMaximum(255);
		connect(slider, SIGNAL(valueChanged(int)), this, SLOT(sliderChanged()));
		sliderLayout->addWidget(l,index, 0);
		sliderLayout->addWidget(slider, index, 1);
		m_sliderList.append(slider);
		m_sliderLabelList.append(l);
		index++;
	}
	mainLayout->addLayout(sliderLayout);
	mainLayout->addStretch(1);
	if (!m_modeless) {
		QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
														   | QDialogButtonBox::Cancel);
		connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
		mainLayout->addWidget(buttonBox);
	}
	this->setLayout(mainLayout);

	// create the background pixmap
	int size = 10;
	QPainter p(&m_backgroundPixmap);
	int w = m_backgroundPixmap.width()/size;
	int h = m_backgroundPixmap.height()/size;
	for (int x=0; x<w; x++) {
		for (int y=0; y<h; y++) {
			if ((x+y)&1)
				p.fillRect(x*size, y*size, 10, 10, Qt::black);
			else
				p.fillRect(x*size, y*size, 10, 10, Qt::white);
		}
	}
	Workspace* ws = m_mainWindow->getWorkspace();
	if (!m_modeless) {
		if (!ws->getColorRect().topLeft().isNull()) {
			move(ws->getColorRect().topLeft());
			resize(ws->getColorRect().size());
		}
	}
	update();
}

ColorEditor::~ColorEditor()
{
	Workspace* ws = m_mainWindow->getWorkspace();
	if (m_modeless)
		ws->setColorModelessRect(QRect(pos(), size()));
	else
		ws->setColorRect(QRect(pos(), size()));
}
void ColorEditor::closeEvent(QCloseEvent* event)
{
	qDebug() << "ColorEditor::closeEvent";
	emit closeSignal();
	QDialog::closeEvent(event);
}

void ColorEditor::editColor(const PovColor& color)
{
	this->m_color = color;
	update();
}

void ColorEditor::update(bool fromSlider)
{
	if (m_inUpdate)		// lock update. QSlider emits a valueChanged() when I set the value, which calls update...
		return;
	m_inUpdate = true;
	QPainter p(&m_paintPixmap);
	p.drawPixmap(0,0, m_backgroundPixmap);
	QColor c(m_color.toQColor());
	p.fillRect(0,0,100,100, c);
	m_paintLabel->setPixmap(m_paintPixmap);
	m_typeList[m_color.m_colorType]->setChecked(true);
	m_dataRed->setText(colorTerm(m_color.redF()));
	m_dataGreen->setText(colorTerm(m_color.greenF()));
	m_dataBlue->setText(colorTerm(m_color.blueF()));
	m_dataFilter->setText(colorTerm(m_color.m_filter));
	m_dataTransmit->setText(colorTerm(m_color.m_transmit));
	bool showFilter = m_color.m_colorType == Type_RGBF || m_color.m_colorType == Type_RGBFT;
	bool showTransmit = m_color.m_colorType == Type_RGBT || m_color.m_colorType == Type_RGBFT;
	m_lablFilter->setVisible(showFilter);
	m_dataFilter->setVisible(showFilter);
	m_sliderLabelList[3]->setVisible(showFilter);
	m_sliderList[3]->setVisible(showFilter);
	m_lablTransmit->setVisible(showTransmit);
	m_dataTransmit->setVisible(showTransmit);
	m_sliderLabelList[4]->setVisible(showTransmit);
	m_sliderList[4]->setVisible(showTransmit);
	if (!fromSlider) {
		m_sliderList[0]->setValue(m_color.red());
		m_sliderList[1]->setValue(m_color.green());
		m_sliderList[2]->setValue(m_color.blue());
		m_sliderList[3]->setValue(m_color.m_filter*255.0);
		m_sliderList[4]->setValue(m_color.m_transmit*255.0);
	}
	m_inUpdate = false;
	emit(colorChanged(m_color));
}

void ColorEditor::labelClicked()
{
	QColor color = QColorDialog::getColor(m_color, this, QString(),
								   QColorDialog::DontUseNativeDialog);
	if (color.isValid()) {
		m_color.setRgb(color.red(), color.green(), color.blue());
		update();
	}
}

void ColorEditor::typeChanged()
{
	int newType = 0;
	foreach(QRadioButton* rb, m_typeList) {
		if (rb->isChecked())
			break;
		newType++;
	}
	m_color.m_colorType = (ColorType)newType;
	update();
}

void ColorEditor::sliderChanged()
{
	QSlider* sl = (QSlider*)sender();
	if (m_sliderList[0] == sl)
		m_color.setRed(sl->value());
	if (m_sliderList[1] == sl)
		m_color.setGreen(sl->value());
	if (m_sliderList[2] == sl)
		m_color.setBlue(sl->value());
	if (m_sliderList[3] == sl)
		m_color.setFilter(sl->value());
	if (m_sliderList[4] == sl)
		m_color.setTransmit(sl->value());
	update(true);
}

///////////////////////////////////////////////////////////////////////////////
