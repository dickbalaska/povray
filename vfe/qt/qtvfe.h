#ifndef _QTVFE_H_
#define _QTVFE_H_

#include <string>
#include <QObject>
#include <boost/thread.hpp>

using std::string;

namespace vfePlatform {
class vfeQtSession;
}

namespace vfe {


using vfePlatform::vfeQtSession;

class QtVfe : public QObject
{
	Q_OBJECT
public:
	explicit QtVfe(QObject *parent = nullptr);

	// from gui to vfe
	void	sendMessageToPovray(const QString& message);

	// from vfe to the gui
	void	sendPovrayTextMessage(const char* command);
	void	sendPovrayTextMessage(const QString& command);
	void	sendPovrayTextMessage(const QString& command, const QString& msg);
	void	sendPovrayWarningMessage(const QString& msg);

//	void	sendPovrayTextMessage(const string& command, const string& msg);
//	void	sendPovrayTextMessage(const string& msg);
	void	sendPovrayGraphicsMessage(const uchar* buff, int size);

signals:
	void emitPovrayTextMessage(const QString& command, const QString& msg);
	void emitPovrayTextMessage(const QString& msg);
	void emitPovrayBinaryMessage(const QByteArray& data);

public slots:

private:
	void	commandVersion();
	void	commandRender(const QString& data);
	void	commandCancel();

	void	parseCommandLine(const QString& cl, int& argc, char**& argv);
	void	sessionErrorExit();
	void	deleteArgv(char**& argv);

	void	printNonStatusMessage(vfeQtSession* session);
	void	printStatusMessage(vfeQtSession* session);
	void	printStatus(vfeQtSession* session);
	void	cancelRender(vfeQtSession* session);

	vfeQtSession*	m_session;
	boost::thread*			m_renderMonThread;

	friend void RenderMonitor(QtVfe* qtVfe, vfeQtSession*& sessionp);
};

}	// namespace vfe

#endif // _QTVFE_H_
