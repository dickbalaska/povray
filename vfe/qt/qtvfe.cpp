
#include <QDebug>
#include <QThread>

#include <iostream>
#include <boost/tokenizer.hpp>

#include "qtoptions.h"
#include "qtgraphics.h"
#include "vfeplatform.h"
#include "qtvfe.h"
#include "base/version_info.h"
#include "backend/povray.h"
#include "config.h"

using namespace std;
using namespace boost;
using povray::qtpov::QtGraphics;

namespace vfe {

const char* s_stream = "stream";
const char* s_stream_fatal	 = "fatal";
const char* s_stream_warning = "warning";

bool gCancelRender = false;

QtVfe::QtVfe(QObject *parent) : QObject(parent)
{
	m_session = NULL;
}

void QtVfe::sendMessageToPovray(const QString& message) {
	qDebug() << "vfe received message from ide: " << message;

	QString command;
	QString data;

	int i = message.indexOf(' ');
	if (i == -1) {
		command = message;
		data = "";
	} else {
		command = message.left(i);
		data = message.mid(i+1);
	}

#ifdef _DEBUG
	qDebug() << "Command = '" << command << "'";
	qDebug() << "Data = '" << data << "'";
#endif
	if (command == "version")
		commandVersion();
//	else if (command == "quit")
//		Quit(hdl);
	else if (command == "render" || command == "r")
		commandRender(data);
	else if (command == "cancel")
		commandCancel();
	else {
		qCritical() << "QtVfe unknown command:" << command << "from:" << message;
	}
}

void QtVfe::sendPovrayTextMessage(const QString& command, const QString& msg)
{
	emitPovrayTextMessage(command, msg);
}

void QtVfe::sendPovrayTextMessage(const QString& command)
{
	emit(emitPovrayTextMessage(command));
}

void QtVfe::sendPovrayTextMessage(const char* command)
{
	emit(emitPovrayTextMessage(command));
}

void QtVfe::sendPovrayWarningMessage(const QString& msg)
{
	emit(emitPovrayTextMessage(s_stream_warning, msg));
}

void QtVfe::sendPovrayGraphicsMessage(const uchar* buff, int size)
{
	QByteArray ba((const char*)buff, size);
	emit(emitPovrayBinaryMessage(ba));
}

void RenderMonitor(QtVfe* qtVfe, vfeQtSession*& sessionp)
{
	vfeQtSession* session = sessionp;
	// main render loop
	session->SetEventMask(stBackendStateChanged | stAnyMessage);  // immediatly notify this event
	vfeStatusFlags    flags;
	while (((flags = session->GetStatus(true, 1000)) & stRenderShutdown) == 0) {
//		ProcessSignal();
		if (gCancelRender) {
			qtVfe->cancelRender(session);
			break;
		}
#ifdef _DEBUG
		qDebug() << "flags: " << hex << flags;
#endif
		if (flags & stAnimationStatus) {
			//stringstream ss;
			QString qs = QString("Rendering frame %1 of %2 (#%3)")
								 .arg(session->GetCurrentFrame())
								 .arg(session->GetTotalFrames())
								 .arg(session->GetCurrentFrameId());
#ifdef _DEBUG
			qDebug() << qs;
#endif
			//ss << "stream 6 Rendering frame " <<  session->GetCurrentFrame() << " of " << session->GetTotalFrames() << " (#" << session->GetCurrentFrameId() << ")";
			//wsSend(hdl, ss.str());
			qs = "6 " + qs;
			qtVfe->emitPovrayTextMessage("stream", qs);
			continue;
		}
		if (flags & stAnyMessage)
			qtVfe->printStatusMessage (session);
		if (flags & stAnyMessage) {
			qtVfe->printNonStatusMessage(session);
		}
		if (flags & stBackendStateChanged) {
			// notification that we are starting parsing or starting rendering. We don't care.
			// This information is handled by qtpovray via other messages.
			// qWarning() << "\nUnhandled PrintStatusChanged\n";
			// ( qtVfe->printStatusChanged(session); )
		}

	}
#ifdef _DEBUG
	cerr << "RenderMonitor break" << endl;
#endif
	// pause when done for single or last frame of an animation
//    if (session->Failed() == false && GetRenderWindow() != NULL && session->GetBoolOption("Pause_When_Done", false))
//    {
//        PrintStatusChanged(session, kPausedRendering);
//        PauseWhenDone(session);
//        gCancelRender = false;
//    }

//    if (running_benchmark)
//        CleanupBenchmark(session, bench_ini_name, bench_pov_name);

//    if (session->Succeeded() == false)
//        retval = gCancelRender ? RETURN_USER_ABORT : RETURN_ERROR;
	session->Shutdown();
	qtVfe->printStatus(session);
//    delete sigthread;
	delete session;
	sessionp = NULL;
	//wsSend(hdl, "done");
	qtVfe->sendPovrayTextMessage("done");

}

QtVfe*	gQtVfe;

static vfeDisplay* QtDisplayCreator (unsigned int width, unsigned int height, GammaCurvePtr gamma, vfeSession *session, bool visible)
{
#ifdef _DEBUG
	cerr << "WsDisplayCreator: w=" << width << " h=" << height << endl;
#endif
	QtGraphics* qtg = new QtGraphics(width, height, gamma, session, visible);
	qtg->setQtVfe(gQtVfe);
	//qtg->m_hdl = gHdl;
	qtg->SendInit();

	return(qtg);
}

void  QtVfe::commandRender(const QString& data)
{
	int argc;
	char** argv;		// XXX These leak
	gCancelRender = false;
	parseCommandLine(data, argc, argv);
	char** oldargv = argv;
#ifdef _DEBUG
	cerr << "chdir: " << argv[0] << endl;
#endif
	int ret = chdir(argv[0]);
	if (ret) {
		QString s = QString("Failed to chdir to '%1'").arg(argv[0]);
		emitPovrayTextMessage(s_stream_fatal, s);
//		stringstream ss;
//		ss << "Failed to chdir to '" << argv[0] << "'" << endl;
//		wsSend(hdl, ss.str());
		return;
	}
	m_session = new vfeQtSession(this);
	m_session->m_renderOptions = new vfeRenderOptions();
	if (m_session->Initialize(NULL, NULL) != vfe::vfeNoError) {

		sessionErrorExit();
		return;
	}

	// default number of work threads: number of CPUs or 4
	int nthreads = 1;
	nthreads = QThread::idealThreadCount();
	if (nthreads < 2)
		nthreads = 4;
	m_session->m_renderOptions->SetThreadCount(nthreads);
	m_session->GetUnixOptions()->ProcessOptions(&argc, &argv);
	m_session->GetUnixOptions()->Process_povray_ini(*m_session->m_renderOptions);
	while (*++argv)
		m_session->m_renderOptions->AddCommand (*argv);

	char** pp = oldargv;
	while (*pp) {
		delete *pp;
		pp++;
	}
	delete oldargv;

	if (m_session->SetOptions(*m_session->m_renderOptions) != vfeNoError) {
		string s = s_stream_fatal;
		s += "Problem with option setting";
//		wsSend(hdl, s);
//        for(int loony=0;loony<argc_copy;loony++)
//        {
//            fprintf(stderr,"%s%c",argv_copy[loony],loony+1<argc_copy?' ':'\n');
//        }
		sessionErrorExit();
		deleteArgv(argv);
		return;
	}
//	gHdl = hdl;
	gQtVfe = this;
	m_session->SetDisplayCreator(QtDisplayCreator);

	if (m_session->StartRender() != vfeNoError) {
		sessionErrorExit();
		deleteArgv(argv);
		return;
	}
	deleteArgv(argv);
	renderMonThread = new boost::thread(RenderMonitor, this, m_session);
}

void  QtVfe::commandCancel()
{
	gCancelRender = true;
}

void QtVfe::parseCommandLine(const QString& cl, int& argc, char**& argv)
{
	string s = cl.toStdString();
	argc = 0;
	escaped_list_separator<char> els('\\', ' ','\"');
	tokenizer<escaped_list_separator<char>> tok(s, els);
	for(tokenizer<escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
		argc++;
	}
	argv = new char*[argc+1];
	char** nargv = argv;
	for(tokenizer<escaped_list_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
		string t = *beg;
		*nargv = new char[t.size()+1];
		strcpy(*nargv, t.c_str());
		nargv++;
	}
	*nargv = NULL;
}

void QtVfe::sessionErrorExit()
{
	QString s = m_session->GetErrorString();
	emitPovrayTextMessage(s_stream_fatal, s);
	//wsSend(hdl, s);
	//fprintf(stderr, "%s\n", session->GetErrorString());
	qWarning() << "ErrorExit: " << s;
	m_session->Shutdown();
	delete m_session;
	m_session = NULL;
	//wsSend(hdl, "done");

	sendPovrayTextMessage("done");
}

void QtVfe::deleteArgv(char**& argv)
{
	char** pp = argv;
	while (*pp) {
		delete *pp;
		pp++;
	}
	//delete argv;
}

void QtVfe::printNonStatusMessage(vfeQtSession* session)
{
	vfeSession::MessageType type;
	string str;
	UCS2String file;
	int	line;
	int	col;

	while (session->GetNextNonStatusMessage (type, str, file, line, col)) {
		QString qs = QString("%1 %2").arg(type).arg(str.c_str());
//		stringstream ss;
//		ss << "stream " << type << " " << str;
#ifdef _DEBUG
//		cerr << "PrintNonStatusMessage: " << ss.str()  << " " << UCS2toASCIIString(file) << " : " << line << " : " << col << endl;
		qDebug() << "PrintNonStatusMessage: " << qs  << " " << UCS2toASCIIString(file).c_str() << " : " << line << " : " << col << endl;
#endif
		//wsSend(hdl, ss.str());
		emitPovrayTextMessage(s_stream, qs);
	}
}

void QtVfe::printStatusMessage(vfeQtSession* session)
{
	vfeSession::MessageType type;
	string str;
	UCS2String file;
	int	line;
	int	col;
	vfeSession::StatusMessage msg(*session);
	while (session->GetNextStatusMessage(msg)) {
		//stringstream ss;
		//ss << type << " " << str << " " << UCS2toASCIIString(file) << " : " << line << " : " << col;
		//if (msg.m_Type == vfeSession::MessageType::mAnimationStatus)
		//	ss << "stream 7 "
		if (msg.m_Type == vfeSession::MessageType::mAnimationStatus)
			continue;
		//ss << "stream " << msg.m_Type << " " << msg.m_Message << " " << UCS2toASCIIString(msg.m_Filename);
		QString qs = QString("%1 %2 %3")
				.arg(msg.m_Type)
				.arg(msg.m_Message.c_str())
				.arg(UCS2toASCIIString(msg.m_Filename).c_str());
		//ss << "stream " << msg.m_Type << " " << msg.m_Message << " " << UCS2toASCIIString(msg.m_Filename);
#ifdef _DEBUG
		qDebug() << "PrintStatusMessage: " << qs;
#endif
		//wsSend(hdl, ss.str());
		emitPovrayTextMessage(s_stream, qs);
	}
}

void QtVfe::printStatus(vfeQtSession* session)
{
	string str;
	vfeSession::MessageType type;
	static vfeSession::MessageType lastType = vfeSession::mUnclassified;

	while (session->GetNextCombinedMessage (type, str)) {
		QString qs = QString("%1 %2").arg(type).arg(str.c_str());
#ifdef _DEBUG
		qDebug() << "PrintStatus:" << qs;
#endif
		emitPovrayTextMessage(s_stream, qs);
	}
}

void QtVfe::cancelRender(vfeQtSession* session)
{
#ifdef _DEBUG
	cerr << "CancelRender" << endl;
#endif
	session->CancelRender();  // request the backend to cancel
	printStatus (session);
	while (session->GetBackendState() != kReady)  // wait for the render to effectively shut down
		Delay(10);
	printStatus(session);
}

void  QtVfe::commandVersion()
{
	// TODO -- GNU/Linux customs would be to print to stdout (among other differences).
	QString s = QString("%1 %2\n\n"
						"%3\n%4\n%5\n"
						"%6\n%7\n%8\n\n")
			.arg("povray").arg(POV_RAY_VERSION)
			.arg(DISTRIBUTION_MESSAGE_1).arg(DISTRIBUTION_MESSAGE_2).arg(DISTRIBUTION_MESSAGE_3)
			.arg(POV_RAY_COPYRIGHT).arg(DISCLAIMER_MESSAGE_1).arg(DISCLAIMER_MESSAGE_2);

	s = QString("%1"
				"Compilation settings:\n"
				"  Build architecture:  %2\n"
//				"  Built/Optimized for: %3\n"
				"  Compiler vendor:     %4\n"
//				"  Compiler version:    %5\n"
				"  Compiler flags:      %6\n\n")
			.arg(s)
			.arg(BUILD_ARCH)
			//.arg(BUILT_FOR)
			.arg(COMPILER_VENDOR)
			//.arg(COMPILER_VERSION)
			.arg(COMPILER_CXXFLAGS);

	s = QString("%1"
				"Host info:\n"
				"  Name:\t\t%2\n"
				"  OS:\t\t%3\n"
				"  Architecture:\t%4\n"
				"  Version:\t\t%5\n"
				"  CPU count:\t%6\n\n")
			.arg(s)
			.arg(HOST_NAME)
			.arg(HOST_OS)
			.arg(HOST_ARCH)
			.arg(HOST_VERSION)
			.arg(HOST_CPUNUM);

	vfeQtSession* tSession = new vfeQtSession(this);
	QString t = tSession->GetUnixOptions()->GetPathsString();
	delete tSession;
	s = s + t;

	emit(emitPovrayTextMessage("version", s));
}
}	// namespace vfe
