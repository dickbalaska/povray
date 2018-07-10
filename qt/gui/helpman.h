/******************************************************************************
 * helpman.h - Manage the Help and About dialog for qtpovray
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
#ifndef _HELPMAN_H_
#define _HELPMAN_H_

#include <QObject>

class QTextEdit;
class MainWindow;

class HelpMan : public QObject
{
	Q_OBJECT
public:
	explicit HelpMan(MainWindow* parent);

	void setPovrayVersion(const QString& text);
signals:

public slots:
	void showAbout();
	void showPovrayHelp();
	void showQtpovrayHelp();
	void showSampleScenes();

private:
	MainWindow*		m_mainWindow;
	QTextEdit*		m_povrayBanner;
};

#endif // _HELPMAN_H_
