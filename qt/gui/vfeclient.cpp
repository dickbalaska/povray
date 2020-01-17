/******************************************************************************
 * wsclient.cpp - The WebSockets client to connect to povrayws
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

#include <QFile>
#include <QProcess>
#include <QDataStream>
#include <QDebug>

#include "vfeclient.h"
#include "qt/qtvfe.h"

using vfe::QtVfe;

VfeClient::VfeClient(bool debug, QObject *parent) :
	QObject(parent)
	,m_debug(debug)
#ifdef USE_WEBSOCKETS
	,povrayProcess(NULL)
	,m_connected(false)
#endif
{
	m_qtVfe = new QtVfe(this);
	connect(m_qtVfe, &QtVfe::emitPovrayBinaryMessage,
			this, &VfeClient::onBinaryMessageReceived);
	connect(m_qtVfe, SIGNAL(emitPovrayTextMessage(QString)),
			this, SLOT(onTextMessageReceived(QString)));
	connect(m_qtVfe, SIGNAL(emitPovrayTextMessage(QString,QString)),
			this, SLOT(onTextMessageReceived(QString,QString)));
}

VfeClient::~VfeClient()
{
	delete m_qtVfe;
}

//void VfeClient::close()
//{
//	if (m_qtVfe) {
//		sendMessage("quit");
//		delete m_qtVfe;
//		m_qtVfe = nullptr;
//	}
//}
void VfeClient::onTextMessageReceived(const QString &msg)
{
	if (m_debug)
		qDebug() << "VfeClient::onTextMessageReceived:" << msg;
	int i = msg.indexOf(' ');
	if (i == -1) {
		//qCritical() << "Malformed message received: " << msg;
		//return;
		emit(messageReceived(msg, ""));
		return;
	}
	QString command(msg.left(i));
	QString text(msg.mid(i+1));
	emit(messageReceived(command, text));
}

void VfeClient::onTextMessageReceived(const QString& command, const QString& msg)
{
	if (m_debug)
		qDebug() << "VfeClient::onTextMessageReceived:" << command << msg;
	emit(messageReceived(command, msg));
}

void VfeClient::sendMessage(const QString& msg)
{
	if (m_debug)
		qDebug() << "VfeClient::sendMessage:" << msg;
	m_qtVfe->sendMessageToPovray(msg);
}

void VfeClient::onBinaryMessageReceived(const QByteArray& data)
{
	emit(binaryMessageReceived(data));
}
