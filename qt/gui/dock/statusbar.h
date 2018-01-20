/******************************************************************************
 * statusbar.h - The statusbar for qtpov.
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

#ifndef _STATUSBAR_H_
#define _STATUSBAR_H_

#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QTime>

class MainWindow;

class StatusBar : public QStatusBar
{
	Q_OBJECT
public:
	explicit StatusBar(MainWindow* parent);

signals:

public slots:
	void	showMessage(const QString &text, int value = -1);
	void	showRenderMessage(const QString &text);
	void	showAnimationMessage(const QString &text);
	void	renderStart();
	void	renderDone();
	void	timerTicked();

private:
	QProgressBar	m_progressBar;
	QLabel			m_statusLabel;
	QLabel			m_animationLabel;
	QProgressBar	m_animationBar;
	QLabel			m_timerLabel;
	QTimer			m_timer;
	QTime			m_startTime;
	bool			m_useAnimationBar;
};

#endif // _STATUSBAR_H_
