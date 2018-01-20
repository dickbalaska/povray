/******************************************************************************
 * filterdialog.cpp - The dialog to filter which files get displayed in the tree
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

#ifndef _FILTERDIALOG_H_
#define _FILTERDIALOG_H_

#include <QDialog>
#include <QRadioButton>
#include "workspace.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QGridLayout;
QT_END_NAMESPACE

class MainWindow;

class MyRadioButton : public QRadioButton
{
	Q_OBJECT
public:
	explicit MyRadioButton(const QString &text, QWidget* parent, FilterFileStyle ffs)
		: QRadioButton(text, parent),
		  filterFileStyle(ffs)
	{}

	FilterFileStyle filterFileStyle;
};

class FilterDialog : public QDialog
{
	Q_OBJECT

public:
	FilterDialog(MainWindow* parent, Workspace *workspace);

private slots:
	void	deleteThis(bool);
	void	addThis(bool);
	void	filterFileStyleChanged(bool);

private:
	void	addFilterWidgets();
	void	removeAllFilters(QLayout *layout);

	MainWindow*	m_mainWindow;
	Workspace* m_workspace;
	QLineEdit*	m_newFilter;
	QGridLayout* m_glayout;
};

#endif // _FILTERDIALOG_H_
