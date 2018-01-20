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

#include "wsclient.h"


WsClient::WsClient(const QUrl &url, bool debug, QObject *parent) :
	QObject(parent),
	m_url(url),
	m_debug(debug),
	povrayProcess(NULL),
	m_connected(false)

{
	if (m_debug)
		qDebug() << "WebSocket server:" << url;
	connect(&m_webSocket, &QWebSocket::connected, this, &WsClient::onConnected);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &WsClient::onDisconnected);
}

WsClient::~WsClient()
{
	if (povrayProcess) {
		povrayProcess->kill();
		povrayProcess = NULL;
	}
}
void WsClient::connectToPovray() {
	if (m_debug)
		qDebug() << "WebSocket connectToPovray";
	m_webSocket.open(m_url);
}

void WsClient::close()
{
	if (!m_url.isEmpty()) {
		m_webSocket.sendTextMessage("quit");
		m_webSocket.close();
	}
}
void WsClient::onConnected()
{
	if (m_debug)
		qDebug() << "WebSocket connected";
	connect(&m_webSocket, &QWebSocket::binaryMessageReceived,
			this, &WsClient::onBinaryMessageReceived);
	connect(&m_webSocket, &QWebSocket::textMessageReceived,
			this, &WsClient::onTextMessageReceived);
	m_connected = true;
	emit(clientStateChanged(true));
	//m_webSocket.sendTextMessage(QStringLiteral("Hello, world!"));
}

void WsClient::onDisconnected()
{
	if (m_debug)
		qDebug() << "Websocket disconnected:" << m_webSocket.errorString();
	m_connected = false;
	emit(clientStateChanged(false));
}
void WsClient::onTextMessageReceived(QString msg)
{
	if (m_debug)
		qDebug() << "Message received:" << msg;
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

void WsClient::sendMessage(const QString& msg)
{
	if (m_debug)
		qDebug() << "Message sent:" << msg;
	m_webSocket.sendTextMessage(msg);
}

void WsClient::onBinaryMessageReceived(const QByteArray& data)
{
	emit(binaryMessageReceived(data));
}
