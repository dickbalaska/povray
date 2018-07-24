/******************************************************************************
 * imagedisplayer.cpp - Display a graphic in an "editor" page
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2018 - Dick Balaska, and BuckoSoft.
 *
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
 *
 *****************************************************************************/

#include <QLabel>

#include "imagedisplayer.h"

ImageDisplayer::ImageDisplayer(QWidget *parent) : QScrollArea(parent)
{

}

void ImageDisplayer::loadImage()
{
	QLabel* l = new QLabel();
	//l->setText(this->m_filePath);
	QImage image(this->m_filePath);
	l->setPixmap(QPixmap::fromImage(image));

	this->setBackgroundRole(QPalette::Dark);
	this->setWidget(l);
}
