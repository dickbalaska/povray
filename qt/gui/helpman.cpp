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

#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
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
#include "preferencedata.h"
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
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QLabel* l;

	QHBoxLayout* hLayout = new QHBoxLayout();
	l = new QLabel();
	l->setPixmap(*mainPixmap);
	l->setScaledContents(true);
	l->setAlignment(Qt::AlignCenter);
	l->setMaximumSize(100,100);
	hLayout->addWidget(l);
	mainLayout->addLayout(hLayout);


	l = new QLabel();
	l->setTextFormat(Qt::RichText);
	l->setText(tr("qtpovray - A POV-Ray&trade; IDE"));
	mainLayout->addWidget(l);
	l = new QLabel(getVersionString());
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

	d.setLayout(mainLayout);
	d.exec();
	m_mainWindow->getWorkspace()->setAboutRect(QRect(d.pos(), d.size()));

	m_povrayBanner = NULL;
}

// Displayed the first time running
bool HelpMan::showVirginSplash()
{
	QDialog d(m_mainWindow);
	d.setWindowTitle(tr("qtpovray splash"));
	QVBoxLayout* mainLayout = new QVBoxLayout();
	QLabel* l;

	QHBoxLayout* hLayout = new QHBoxLayout();
	l = new QLabel();
	l->setPixmap(*mainPixmap);
	l->setScaledContents(true);
	l->setAlignment(Qt::AlignCenter);
	l->setMaximumSize(100,100);
	hLayout->addWidget(l);
	mainLayout->addLayout(hLayout);

	l = new QLabel();
	l->setTextFormat(Qt::RichText);
	l->setText(tr("Welcome to qtpovray - A POV-Ray&trade; IDE"));
	mainLayout->addWidget(l);

	l = new QLabel(getVersionString());
	mainLayout->addWidget(l);

	l = new QLabel();
	l->setText("<a href=\"http://www.buckosoft.com/qtpovray/\">www.buckosoft.com/qtpovray</a>");
	l->setTextFormat(Qt::RichText);
	l->setTextInteractionFlags(Qt::TextBrowserInteraction);
	l->setOpenExternalLinks(true);
	mainLayout->addWidget(l);

	l = new QLabel("<hr>");
	mainLayout->addWidget(l);

	l = new QLabel(tr("In the following dialog, create a file to store your workspace in."));
	mainLayout->addWidget(l);
	l = new QLabel(tr("This is just metadata about your project, not your POV-Ray source."));
	mainLayout->addWidget(l);
	l = new QLabel(tr("You will specify the location of your POV-Ray source later."));
	mainLayout->addWidget(l);

	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, &d, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, &d, &QDialog::reject);
	mainLayout->addWidget(buttonBox);
	d.setLayout(mainLayout);
	int ret = d.exec();
	qDebug() << "Splash returned" << ret;
	return(ret == QDialog::Accepted);
}

void HelpMan::showSampleScenes()
{
	const PreferenceData& pd = m_mainWindow->getPreferenceData();
	QDir dir(pd.getPovraySceneDirectory());
	QFileInfo fi(dir, "index.htm");
	if (!fi.exists()) {
		QMessageBox::critical(m_mainWindow, tr("No help"), tr("Can't open POV-Ray sample scenes.\n File %1 does not exist.").arg(fi.absoluteFilePath()));
		return;
	}
	QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
	QDesktopServices::openUrl(url);
}

void HelpMan::showQtpovrayHelp()
{
	const PreferenceData& pd = m_mainWindow->getPreferenceData();
	QDir dir(pd.getQtpovrayHelpDirectory());
	QFileInfo fi(dir, "index.html");
	if (!fi.exists()) {
		QMessageBox::critical(m_mainWindow, tr("No help"), tr("Can't open qtpovray help.\n File %1 does not exist.").arg(fi.absoluteFilePath()));
		return;
	}
	QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
	QDesktopServices::openUrl(url);
}

void HelpMan::showPovrayHelp()
{
	const PreferenceData& pd = m_mainWindow->getPreferenceData();
	QDir dir(pd.getPovrayHelpDirectory());
	QFileInfo fi(dir, "index.html");
	if (!fi.exists()) {
		QMessageBox::critical(m_mainWindow, tr("No help"), tr("Can't open POV-Ray help.\n File %1 does not exist.").arg(fi.absoluteFilePath()));
		return;
	}
	QUrl url = QUrl::fromLocalFile(fi.absoluteFilePath());
	QDesktopServices::openUrl(url);
}

void HelpMan::setPovrayVersion(const QString& text)
{
	if (m_povrayBanner) {
		m_povrayBanner->setText(text);
	}
}

QString HelpMan::getVersionString()
{
	QString v(tr("Version %1").arg(VERSION));
#ifdef _DEBUG
	v += " - DEBUG";
#endif
	return(v);
}
