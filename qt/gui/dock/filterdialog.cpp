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

#include <QtWidgets>
#include <QDebug>

#include "mainwindow.h"
#include "workspace.h"
#include "dockman.h"
#include "resourcedock.h"
#include "preferencedata.h"
#include "filterdialog.h"

QIcon* iconDelete;
QIcon* iconAdd;


FilterDialog::FilterDialog(MainWindow *parent, Workspace* workspace)
	: QDialog(parent),
	  m_mainWindow(parent),
	  m_workspace(workspace)
{
	setWindowTitle(tr("Resource Filters"));
	iconDelete	= new QIcon(":/resources/icons/cross.png");
	iconAdd		= new QIcon(":/resources/icons/add.png");

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setObjectName("filters:mainLayout");

	QVBoxLayout* layout = new QVBoxLayout();
	FilterFileStyle ffs = workspace->getResourceFilterType();
	MyRadioButton* rb = new MyRadioButton(tr("Show all files"), this, FilterNone);
	connect(rb, SIGNAL(clicked(bool)), this, SLOT(filterFileStyleChanged(bool)));
	layout->addWidget(rb);
	if (ffs == FilterNone)
		rb->setChecked(true);
	rb = new MyRadioButton(tr("Dim filtered files"), this, FilterDim);
	connect(rb, SIGNAL(clicked(bool)), this, SLOT(filterFileStyleChanged(bool)));
	layout->addWidget(rb);
	if (ffs == FilterDim)
		rb->setChecked(true);
	rb = new MyRadioButton(tr("Hide filtered files"), this, FilterHide);
	connect(rb, SIGNAL(clicked(bool)), this, SLOT(filterFileStyleChanged(bool)));
	layout->addWidget(rb);
	if (ffs == FilterHide)
		rb->setChecked(true);
	mainLayout->addLayout(layout);

	m_glayout = new QGridLayout();
	m_glayout->setObjectName("filters:glayout");
	addFilterWidgets();
	mainLayout->addLayout(m_glayout);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	//connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	mainLayout->addWidget(buttonBox);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief FilterDialog::addFilterWidgets
/// Add the labels and their delete buttons in the grid,
/// followed by the newFilter widget
void FilterDialog::addFilterWidgets()
{
	int row = 0;
	QStringList sl = m_mainWindow->getWorkspace()->getResourceFilters();
	foreach (QString s , sl) {
		m_glayout->addWidget(new QLabel(s, this), row, 0);
		QToolButton* deleteMe = new QToolButton(this);
		deleteMe->setIcon(*iconDelete);
		deleteMe->setToolTip(tr("Remove ") + s);	// How do I mark which button was pressed? Sleezy to use tooltips.
		connect(deleteMe, SIGNAL(clicked(bool)), this, SLOT(deleteThis(bool)));
		m_glayout->addWidget(deleteMe, row, 1);
		row++;
	}
	m_newFilter = new QLineEdit(this);
	m_glayout->addWidget(m_newFilter, row, 0);
	QToolButton* addMe = new QToolButton(this);
	addMe->setIcon(*iconAdd);
	connect(addMe, SIGNAL(clicked(bool)), this, SLOT(addThis(bool)));
	m_glayout->addWidget(addMe, row, 1);
}

void FilterDialog::removeAllFilters(QLayout* layout)
{
	bool deleteWidgets = true;
	while (QLayoutItem* item = layout->takeAt(0))
	{
		QWidget* widget;
		if (  (deleteWidgets)
			  && (widget = item->widget())  ) {
			delete widget;
		}
		if (QLayout* childLayout = item->layout()) {
			removeAllFilters(childLayout);
		}
		delete item;
	}
}

void FilterDialog::deleteThis(bool)
{
	QToolButton* pb = qobject_cast<QToolButton*>(sender());
	QString f = tr("Remove ");
	QString s = pb->toolTip().mid(f.length());
	//qDebug() << "deleteThis:" << s;
	m_workspace->getResourceFilters().removeOne(s);
	m_mainWindow->getDockMan()->getResourceDock()->m_fileSystemModel->setNameFilters(
				m_workspace->getResourceFilters());
	//m_glayout->
	removeAllFilters(m_glayout);
	addFilterWidgets();
}

void	FilterDialog::addThis(bool)
{
	QString s = m_newFilter->text();
	if (s.isEmpty())
		return;
	if (m_workspace->getResourceFilters().contains(s)) {
		// XXX put up a messagebox here
		return;
	}
	m_workspace->getResourceFilters().append(s);
	m_mainWindow->getDockMan()->getResourceDock()->m_fileSystemModel->setNameFilters(
				m_workspace->getResourceFilters());
	m_newFilter->setText("");
	removeAllFilters(m_glayout);
	addFilterWidgets();
//	qDebug() << "newFilter:" << f;
}

void FilterDialog::filterFileStyleChanged(bool)
{
	MyRadioButton* button = qobject_cast<MyRadioButton*>(sender());
	m_mainWindow->getWorkspace()->setResourceFilterType(button->filterFileStyle);
	m_mainWindow->getDockMan()->getResourceDock()->setupFilters();

	//qDebug() << "ffs";
}
