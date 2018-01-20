/******************************************************************************
 * rendercl.h - The toolbar dropdown widget that the user types render options into
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

#ifndef _RENDERCL_H_
#define _RENDERCL_H_

#include <QComboBox>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
class QKeyEvent;
QT_END_NAMESPACE

class Dropdown : public QComboBox
{
	Q_OBJECT
public:
	Dropdown(QWidget *parent = Q_NULLPTR);
	~Dropdown() override {}

	QString getValue();
	void	setValue(const QString& s);

Q_SIGNALS:
	void enterPressed();

protected:
	virtual void keyReleaseEvent(QKeyEvent* e);

private:
	void	updateValue(const QString& value);

};

#endif // _RENDERCL_H_
