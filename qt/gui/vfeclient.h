/******************************************************************************
 * vfeclient.h - The client to connect the gui to povray's vfe
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

#ifndef VFECLIENT_H_
#define VFECLIENT_H_

#include <QObject>

QT_BEGIN_NAMESPACE
class QProcess;
QT_END_NAMESPACE

namespace vfe {
class QtVfe;
}
using vfe::QtVfe;

enum MsgType {
	mUnclassified = 0,
	mDebug,
	mInformation,
	mWarning,
	mPossibleError,
	mError,
	mAnimationStatus,
	mGenericStatus,
	mDebugger,
	mDivider	
};

class VfeClient : public QObject
{
	Q_OBJECT
public:
	explicit VfeClient(bool debug = false, QObject *parent = Q_NULLPTR);
	virtual ~VfeClient();

	const QString& getWsErrorMsg() { return(wsErrorMsg); }
//	void setPovrayProcess(QProcess* p) { povrayProcess = p; }
	virtual void connectToPovray() {}
//	bool isConnected() { return(m_connected); }
//	void close();
	void sendMessage(const QString& msg);

Q_SIGNALS:
	void clientStateChanged(bool connected);
	void messageReceived(const QString& command, const QString& message);
	void binaryMessageReceived(const QByteArray& data);

private Q_SLOTS:
	//void onConnected();
	//void onDisconnected();
	void onTextMessageReceived(const QString& msg);
	void onTextMessageReceived(const QString& command, const QString& msg);
	void onBinaryMessageReceived(const QByteArray& data);

private:
//	QWebSocket m_webSocket;
	bool	m_debug;
	//	QUrl	m_url;
	QString	wsErrorMsg = "";
//	QProcess*	povrayProcess;
//	bool	m_connected;
	QtVfe*	m_qtVfe;
};

#endif // VFECLIENT_H_
