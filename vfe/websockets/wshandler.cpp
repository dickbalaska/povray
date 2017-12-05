/*
 * wshandler.cpp
 *
 *  Created on: Nov 13, 2017
 *      Author: dick
 */

#include <mutex>
#include <condition_variable>
#include <boost/tokenizer.hpp>

#include "base/version_info.h"
#include "backend/povray.h"

//#include "config.h"
#include "vfeplatform.h"
#include "wshandler.h"
#include "wsgraphics.h"

extern void ProcessSignal(void);
extern bool gCancelRender;

namespace povray {
namespace websockets {

using namespace std;
using namespace boost;

using namespace vfe;
using namespace vfePlatform;

WsHandler websocketHandler;

static websocketpp::connection_hdl gHdl;	// pass the handle to the display creator

const char* s_stream_fatal = "stream fatal ";

std::mutex shutdownMtx;
std::condition_variable shutdownMtx_cv;

bool willShutdown = false;
bool isShutdown() { return(willShutdown); }

void notifyShutdown() {
    std::unique_lock<std::mutex> lck(shutdownMtx);
    willShutdown = true;
    shutdownMtx_cv.notify_one();
}
void waitForShutdown() {
	if (!isShutdown()) {
		std::unique_lock<std::mutex> lck(shutdownMtx);
		shutdownMtx_cv.wait(lck, isShutdown);
	}

}
void WsHandler::staticReceiveHandler(websocketpp::connection_hdl hdl, message_ptr msg) {
	websocketHandler.on_message(hdl, msg);
}

WsHandler::WsHandler()
	: session(NULL),
	  renderMonThread(NULL)
{
	// TODO Auto-generated constructor stub

}

WsHandler::~WsHandler() {
	// TODO Auto-generated destructor stub
}

void wsSend(websocketpp::connection_hdl hdl, const char* msg) {
    try {
    	WebsocketServer::send(hdl, msg);
    } catch (const websocketpp::lib::error_code& e) {
        std::cerr << "send failed because: " << e
                  << "(" << e.message() << ")" << std::endl;
    }
}

void WsHandler::on_message(websocketpp::connection_hdl hdl, message_ptr msg)
{
	string s = msg->get_payload();
//	tokenizer<> tok(s);
//	tokenizer<>::iterator beg=tok.begin();
//	string command = *beg;
	string command;
	string data;
	int i = s.find(' ');
	if (i == -1) {
		command = s;
		data = "";
	} else {
		command = s.substr(0, i);
		data = s.substr(i+1);
	}
	cerr << "Command = '" << command << "'" << endl;
	cerr << "Data = '" << data << "'" << endl;
	if (command == "version")
		PrintVersion(hdl);
	else if (command == "quit")
		Quit(hdl);
	else if (command == "render" || command == "r")
		Render(hdl, data);
	else if (command == "cancel")
		Cancel(hdl);
	else {
		s = "unknown command: ";
		s += command;
		wsSend(hdl, s);
		cerr << s << endl;
	}
}
void WsHandler::Quit(websocketpp::connection_hdl hdl)
{
	cerr << "exit on user command" << endl;
	notifyShutdown();
}

void WsHandler::Cancel(websocketpp::connection_hdl hdl)
{
#ifdef _DEBUG
	cerr << "Cancel on user command" << endl;
#endif
	gCancelRender = true;
}


void WsHandler::PrintVersion(websocketpp::connection_hdl hdl)
{
	string s = GetBannerVersion();
	wsSend(hdl, s);
}

void PrintNonStatusMessage(websocketpp::connection_hdl hdl, vfeWebsocketSession* session)
{
    vfeSession::MessageType type;
	string str;
	UCS2String file;
	int	line;
	int	col;

	while (session->GetNextNonStatusMessage (type, str, file, line, col)) {
    	stringstream ss;
    	ss << "stream " << type << " " << str;
#ifdef _DEBUG
    	cerr << "PrintNonStatusMessage: " << ss.str()  << " " << UCS2toASCIIString(file) << " : " << line << " : " << col << endl;
#endif
    	wsSend(hdl, ss.str());
	}
}
void PrintStatusMessage(websocketpp::connection_hdl hdl, vfeWebsocketSession* session)
{
    vfeSession::MessageType type;
	string str;
	UCS2String file;
	int	line;
	int	col;
	vfeSession::StatusMessage msg(*session);
	while (session->GetNextStatusMessage(msg)) {
    	stringstream ss;
    	//ss << type << " " << str << " " << UCS2toASCIIString(file) << " : " << line << " : " << col;
    	//if (msg.m_Type == vfeSession::MessageType::mAnimationStatus)
    	//	ss << "stream 7 "
    	if (msg.m_Type == vfeSession::MessageType::mAnimationStatus)
    		continue;
    	ss << "stream " << msg.m_Type << " " << msg.m_Message << " " << UCS2toASCIIString(msg.m_Filename);
#ifdef _DEBUG
    	cerr << "PrintStatusMessage: " << ss.str() << endl;
#endif
    	wsSend(hdl, ss.str());
	}
}

void PrintStatus(websocketpp::connection_hdl hdl, vfeWebsocketSession* session)
{
    string str;
    vfeSession::MessageType type;
    static vfeSession::MessageType lastType = vfeSession::mUnclassified;

    while (session->GetNextCombinedMessage (type, str)) {
    	stringstream ss;
    	ss << "stream " << type << " " << str;
#ifdef _DEBUG
    	cerr << "PrintStatus: " << ss.str() << endl;
#endif
    	wsSend(hdl, ss.str());
    }
}
void PrintStatusChanged (websocketpp::connection_hdl hdl, vfeSession *session, State force = kUnknown)
{
#ifdef _DEBUG
    if (force == kUnknown)
        force = session->GetBackendState();
    switch (force)
    {
        case kParsing:
            fprintf (stderr, "==== [Parsing...] ==========================================================\n");
            break;
        case kRendering:
            fprintf (stderr, "==== [Rendering...] ========================================================\n");
            break;
        case kPausedRendering:
            fprintf (stderr, "==== [Paused...] ===========================================================\n");
            break;
    }
#endif
}

void CancelRender(websocketpp::connection_hdl hdl, vfeWebsocketSession* session)
{
#ifdef _DEBUG
	cerr << "CancelRender" << endl;
#endif
    session->CancelRender();  // request the backend to cancel
    PrintStatus (hdl, session);
    while (session->GetBackendState() != kReady)  // wait for the render to effectively shut down
        Delay(10);
    PrintStatus (hdl, session);
}

void WsHandler::ErrorExit(websocketpp::connection_hdl hdl)
{
	string s = s_stream_fatal;
	s += session->GetErrorString();
	wsSend(hdl, s);
    //fprintf(stderr, "%s\n", session->GetErrorString());
	cerr << "ErrorExit: " << s << endl;
    session->Shutdown();
    delete session;
    session = NULL;
    wsSend(hdl, "done");
}

void WsHandler::ParseCommandLine(const string& s, int& argc, char**& argv)
{
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
void WsHandler::DeleteArgv(char**& argv)
{
	char** pp = argv;
	while (*pp) {
		delete *pp;
		pp++;
	}
	//delete argv;
}

static vfeDisplay *WsDisplayCreator (unsigned int width, unsigned int height, GammaCurvePtr gamma, vfeSession *session, bool visible)
{
#ifdef _DEBUG
	cerr << "WsDisplayCreator: w=" << width << " h=" << height << endl;
#endif
	WsGraphics* wsg = new WsGraphics(width, height, gamma, session, visible);
	wsg->m_hdl = gHdl;
	wsg->SendInit();

	return(wsg);
}

void WsHandler::Render(websocketpp::connection_hdl hdl, const string& data)
{
	int argc;
	char** argv;		// XXX These leak
	gCancelRender = false;
	ParseCommandLine(data, argc, argv);
	char** oldargv = argv;
	cerr << "chdir: " << argv[0] << endl;
	int ret = chdir(argv[0]);
	if (ret) {
		stringstream ss;
		ss << "Failed to chdir to '" << argv[0] << "'" << endl;
		wsSend(hdl, ss.str());
		return;
	}
    session = new vfeWebsocketSession(hdl);
	session->renderOptions = new vfeRenderOptions();
    if (session->Initialize(NULL, NULL) != vfeNoError) {
        ErrorExit(hdl);
        return;
    }

    // default number of work threads: number of CPUs or 4
    int nthreads = 1;
#ifdef _SC_NPROCESSORS_ONLN  // online processors
    nthreads = sysconf(_SC_NPROCESSORS_ONLN);
#endif
#ifdef _SC_NPROCESSORS_CONF  // configured processors
    if (nthreads < 2)
        nthreads = sysconf(_SC_NPROCESSORS_CONF);
#endif
#ifdef SYSTEM_INFO
	SYSTEM_INFO           sysinfo;
	GetSystemInfo(&sysinfo);
	nthreads = sysinfo.dwNumberOfProcessors;
#endif

    if (nthreads < 2)
        nthreads = 4;
    session->renderOptions->SetThreadCount(nthreads);
    session->GetUnixOptions()->ProcessOptions(&argc, &argv);
    session->GetUnixOptions()->Process_povray_ini(*session->renderOptions);
    while (*++argv)
    	session->renderOptions->AddCommand (*argv);

	char** pp = oldargv;
	while (*pp) {
		delete *pp;
		pp++;
	}
	delete oldargv;

    if (session->SetOptions(*session->renderOptions) != vfeNoError)
    {
    	string s = s_stream_fatal;
    	s += "Problem with option setting";
    	wsSend(hdl, s);
//        for(int loony=0;loony<argc_copy;loony++)
//        {
//            fprintf(stderr,"%s%c",argv_copy[loony],loony+1<argc_copy?' ':'\n');
//        }
        ErrorExit(hdl);
        DeleteArgv(argv);
        return;
    }
    gHdl = hdl;
    session->SetDisplayCreator(WsDisplayCreator);

    if (session->StartRender() != vfeNoError) {
        ErrorExit(hdl);
        DeleteArgv(argv);
        return;
    }
    DeleteArgv(argv);
    renderMonThread = new boost::thread(RenderMonitor, hdl, session);
    //RenderMonitor(hdl, session);
}

void RenderMonitor(websocketpp::connection_hdl hdl, vfeWebsocketSession*& sessionp)
{
	vfeWebsocketSession* session = sessionp;
    // main render loop
    session->SetEventMask(stBackendStateChanged | stAnyMessage);  // immediatly notify this event
    vfeStatusFlags    flags;
    while (((flags = session->GetStatus(true, 1000)) & stRenderShutdown) == 0)
    {
        ProcessSignal();
        if (gCancelRender)
        {
            CancelRender(hdl, session);
            break;
        }
#ifdef _DEBUG
        cerr << "flags: " << hex << flags << endl;
#endif
        if (flags & stAnimationStatus) {
        	stringstream ss;
#ifdef _DEBUG
            fprintf(stderr, "\nRendering frame %d of %d (#%d)\n", session->GetCurrentFrame(), session->GetTotalFrames(), session->GetCurrentFrameId());
#endif
        	ss << "stream 6 Rendering frame " <<  session->GetCurrentFrame() << " of " << session->GetTotalFrames() << " (#" << session->GetCurrentFrameId() << ")";
        	wsSend(hdl, ss.str());
        	continue;
        }
        if (flags & stAnyMessage)
        	PrintStatusMessage (hdl, session);
        if (flags & stAnyMessage) {
        	PrintNonStatusMessage(hdl, session);
        }
        if (flags & stBackendStateChanged) {
           PrintStatusChanged (hdl, session);
        	//fprintf(stderr, "\nUnhandled PrintStatusChanged\n");
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
    PrintStatus (hdl, session);
//    delete sigthread;
    delete session;
    sessionp = NULL;
    wsSend(hdl, "done");

}

}} /* namespace websockets, namespace povray */
