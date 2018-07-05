/******************************************************************************
 * helpman.cpp - Manage the Help and About dialog for qtpovray
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
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

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QDesktopServices>

#include "mainwindow.h"
#include "helpman.h"
#include "workspace.h"
#include "qtpovrayversion.h"

HelpMan::HelpMan(MainWindow* parent)
	: QObject(parent),
	  m_mainWindow(parent)
{

}

void HelpMan::showAbout()
{
	QDialog d(m_mainWindow);
	d.setWindowTitle("About qtpovray");
	QVBoxLayout* mainLayout = new QVBoxLayout(&d);
	QLabel* l = new QLabel(tr("qtpovray - A multiplatform POV-Ray IDE"));
	mainLayout->addWidget(l);
	QString v(tr("Version %1").arg(VERSION));
#ifdef _DEBUG
	v += " - DEBUG";
#endif
	l = new QLabel(v);
	mainLayout->addWidget(l);
#pragma GCC diagnostic ignored "-Wdate-time"
	QString compilationTime = QString("Built on: %1 %2").arg(__DATE__).arg(__TIME__);
	l = new QLabel(compilationTime);
	mainLayout->addWidget(l);
	l = new QLabel();
	l->setText("<a href=\"http://www.buckosoft.com/qtpovray/\">www.buckosoft.com/qtpovray</a>");
	l->setTextFormat(Qt::RichText);
	l->setTextInteractionFlags(Qt::TextBrowserInteraction);
	l->setOpenExternalLinks(true);
	mainLayout->addWidget(l);

	QGroupBox* gridGroupBox = new QGroupBox(tr("POV-Ray Banner"));
	m_povrayBanner = new QTextEdit(&d);
	m_povrayBanner->setReadOnly(true);
	QGridLayout* layout = new QGridLayout(&d);
	layout->addWidget(m_povrayBanner);
	gridGroupBox->setLayout(layout);
	mainLayout->addWidget(gridGroupBox);
	m_mainWindow->sendPovrayMessage("version");
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	connect(buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);
	mainLayout->addWidget(buttonBox);
	QRect rect = m_mainWindow->getWorkspace()->getAboutRect();
	if (!rect.topLeft().isNull()) {
		d.move(rect.topLeft());
		d.resize(rect.size());
	}

	d.exec();
	m_mainWindow->getWorkspace()->setAboutRect(QRect(d.pos(), d.size()));

	m_povrayBanner = NULL;
}

void HelpMan::showSampleScenes()
{
	QUrl url = QUrl::fromLocalFile("/usr/share/qtpovray-3.8/scenes/index.htm");
	QDesktopServices::openUrl(url);
}
void HelpMan::showPovrayHelp()
{
	QUrl url = QUrl::fromLocalFile("/usr/share/qtpovray-3.8/html/index.html");
	QDesktopServices::openUrl(url);
}

void HelpMan::setPovrayVersion(const QString& text)
{
	if (m_povrayBanner) {
		m_povrayBanner->setText(text);
	}
}
