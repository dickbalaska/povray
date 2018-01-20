/******************************************************************************
 * rendercl.cpp - The toolbar dropdown widget that the user types render options into
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

#include <QSizePolicy>
#include <QKeyEvent>
#include <QDebug>

#include "dropdown.h"

Dropdown::Dropdown(QWidget* parent)
	: QComboBox(parent)
{
	this->setObjectName("RenderCL");
	this->setEditable(true);
	this->setInsertPolicy(QComboBox::InsertAtTop);
	QSizePolicy qsp = this->sizePolicy();
	qsp.setHorizontalPolicy(QSizePolicy::MinimumExpanding);
	this->setSizePolicy(qsp);
}

QString Dropdown::getValue()
{
	QString s = currentText();
	updateValue(s);
	setCurrentIndex(0);
	return(s);
}

void Dropdown::setValue(const QString& text)
{
	updateValue(text);
	setCurrentIndex(0);
}

void Dropdown::updateValue(const QString& value)
{
	this->setInsertPolicy(QComboBox::InsertAtTop);
	if (this->count() == 0)
		addItem(value);
	else {
		QString t = this->itemText(0);
		if (value != t) {
			int i = findText(value);
			if (i >= 0)
				removeItem(i);
			insertItem(0, value);
		}
	}
}

void Dropdown:: keyReleaseEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Return)
		emit(enterPressed());
	else
		QComboBox::keyReleaseEvent(e);
}
