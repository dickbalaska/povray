/******************************************************************************
 * statusbar.cpp - The statusbar for qtpov.
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
#include <QRegularExpression>
#include <QDebug>

#include "statusbar.h"

StatusBar::StatusBar(MainWindow *parent)
	: QStatusBar((QWidget*)parent),
	  m_progressBar(this),
	  m_statusLabel(this),
	  m_animationLabel(this),
	  m_animationBar(this),
	  m_timerLabel(this),
	  m_timer(this)
{
	m_useAnimationBar = true;
	addWidget(&m_progressBar);
	addWidget(&m_statusLabel);
	m_progressBar.setTextVisible(true);
	m_progressBar.setRange(0,100);
//	m_progressBar.setAlignment(Qt::AlignCenter);
	m_progressBar.setValue(0);
	m_progressBar.setVisible(false);
	m_timerLabel.setText("00:00");
	m_animationBar.setRange(0,100);
	m_animationBar.setValue(0);
	addPermanentWidget(&m_animationLabel);
	addPermanentWidget(&m_animationBar);
	addPermanentWidget(&m_timerLabel);
	m_animationLabel.setVisible(false);
	m_animationBar.setVisible(false);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(timerTicked()));
}

void StatusBar::showMessage(const QString &text, int value)
{
	if (value == -1) {
		if (!m_statusLabel.isVisible()) {
			m_progressBar.setVisible(false);
			m_statusLabel.setVisible(true);
		}
		m_statusLabel.setText(text);
	} else {
		if (!m_progressBar.isVisible()) {
			m_statusLabel.setVisible(false);
			m_progressBar.setVisible(true);
		}
		m_progressBar.setFormat(text);
		m_progressBar.setValue(value);
	}
}

void StatusBar::showRenderMessage(const QString &text)
{
	int value = 0;
	int x = text.indexOf("(");
	if (x != -1) {
		int y = text.indexOf("%", x);
		if (y != -1) {
			//qDebug() << "foo:" << text.mid(x+1, y-x-1);
			value = text.mid(x+1, y-x-1).toInt();
		}
	}
	showMessage(text, value);
}

void StatusBar::showAnimationMessage(const QString &text)
{
	if (!m_useAnimationBar) {
		if (!m_animationLabel.isVisible())
			m_animationLabel.setVisible(true);
		m_animationLabel.setText(text);
	} else {
		if (!m_animationBar.isVisible())
			m_animationBar.setVisible(true);
		// Rendering frame mmmm of nnnn (#xxxx)
		QRegularExpression re("(\\d+) of (\\d+)");
		QRegularExpressionMatch match = re.match(text);
		QString mm = match.captured(1);
		QString nn = match.captured(2);
		int m = mm.toInt();
		int n = nn.toInt();
		//qDebug() << "rex:" << m << n;
		m_animationBar.setFormat(text);
		m_animationBar.setRange(0, n);
		m_animationBar.setValue(m);
	}
}

void StatusBar::renderStart()
{
	m_startTime.setHMS(0,0,0);
	m_timer.start(1000);
}

void StatusBar::renderDone()
{
	if (m_animationLabel.isVisible()) {
		m_animationLabel.setText("");
		m_animationLabel.setVisible(false);
	}
	m_animationBar.setVisible(false);
	m_timer.stop();

}

void StatusBar::timerTicked()
{
	m_startTime = m_startTime.addSecs(1);
	if (m_startTime.hour())
		m_timerLabel.setText(m_startTime.toString("h:mm:ss"));
	else
		m_timerLabel.setText(m_startTime.toString("mm:ss"));
}
