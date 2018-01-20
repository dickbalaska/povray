/******************************************************************************
 * wsclient.h - The WebSockets client to connect to povrayws
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

#ifndef _WSCLIENT_H_
#define _WSCLIENT_H_

#include <QObject>
#include <QtWebSockets/QWebSocket>

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE


class WsClient : public QObject
{
	Q_OBJECT
public:
	explicit WsClient(const QUrl &url, bool debug = false, QObject *parent = Q_NULLPTR);
	virtual ~WsClient();

	const QString& getWsErrorMsg() { return(wsErrorMsg); }
	void setPovrayProcess(QProcess* p) { povrayProcess = p; }
	void connectToPovray();
	bool isConnected() { return(m_connected); }
	void close();
	void sendMessage(const QString& msg);

Q_SIGNALS:
	void clientStateChanged(bool connected);
	void messageReceived(const QString& command, const QString& message);
	void binaryMessageReceived(const QByteArray& data);

private Q_SLOTS:
	void onConnected();
	void onDisconnected();
	void onTextMessageReceived(QString msg);
	void onBinaryMessageReceived(const QByteArray& data);

private:
	QWebSocket m_webSocket;
	QUrl	m_url;
	bool	m_debug;
	QString	wsErrorMsg = "";
	QProcess*	povrayProcess;
	bool	m_connected;
};

#endif // _WSCLIENT_H_
