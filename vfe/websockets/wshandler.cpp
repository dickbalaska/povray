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

#include "config.h"
#include "wshandler.h"

extern void ProcessSignal(void);
extern bool gCancelRender;

namespace povray {
namespace websockets {

using namespace std;
using namespace boost;

using namespace vfe;
using namespace vfePlatform;

WsHandler websocketHandler;

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
	: opts(NULL),
	  session(NULL),
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
	else {
		s = "unknown command: ";
		s += command;
		wsSend(hdl, s);
	}
}
void WsHandler::Quit(websocketpp::connection_hdl hdl)
{
	cerr << "exit on user command" << endl;
	notifyShutdown();
}

void WsHandler::PrintVersion(websocketpp::connection_hdl hdl)
{
	string v = "version ";
	char s[2048];
    snprintf(s, 2048,
        "%s %s\n\n"
        "%s\n%s\n%s\n"
        "%s\n%s\n%s\n\n",
        PACKAGE_NAME, POV_RAY_VERSION,
        DISTRIBUTION_MESSAGE_1, DISTRIBUTION_MESSAGE_2, DISTRIBUTION_MESSAGE_3,
        POV_RAY_COPYRIGHT, DISCLAIMER_MESSAGE_1, DISCLAIMER_MESSAGE_2
    );
	v += s;
    snprintf(s, 2048,
        "Built-in features:\n"
        "  I/O restrictions:          %s\n"
        "  X Window display:          %s\n"
        "  Supported image formats:   %s\n"
        "  Unsupported image formats: %s\n\n",
        BUILTIN_IO_RESTRICTIONS, BUILTIN_XWIN_DISPLAY, BUILTIN_IMG_FORMATS, MISSING_IMG_FORMATS
    );
	v += s;
    snprintf(s, 2048,
        "Compilation settings:\n"
        "  Build architecture:  %s\n"
        "  Built/Optimized for: %s\n"
        "  Compiler vendor:     %s\n"
        "  Compiler version:    %s\n"
        "  Compiler flags:      %s\n",
        BUILD_ARCH, BUILT_FOR, COMPILER_VENDOR, COMPILER_VERSION, CXXFLAGS
    );
	v += s;
	wsSend(hdl, v);
}

void PrintStatus(websocketpp::connection_hdl hdl, vfeWebsocketSession* session)
{
    string str;
    vfeSession::MessageType type;
    static vfeSession::MessageType lastType = vfeSession::mUnclassified;

    while (session->GetNextCombinedMessage (type, str))
    {
//        if (type != vfeSession::mGenericStatus)
//        {
//            if (lastType == vfeSession::mGenericStatus)
//                fprintf (stderr, "\n") ;
//            fprintf (stderr, "%s\n", str.c_str());
//        }
//        else
//            fprintf (stderr, "%s\r", str.c_str());
//        lastType = type;
    	stringstream ss;
    	ss << type << " " << str;
    	wsSend(hdl, ss.str());
    	cout << "PrintStatus: " << ss.str() << endl;
    }
}
void PrintStatusChanged (websocketpp::connection_hdl hdl, vfeSession *session, State force = kUnknown)
{
    if (force == kUnknown)
        force = session->GetBackendState();
    switch (force)
    {
        case kParsing:
            fprintf (stderr, "==== [Parsing...] ==========================================================\n");
            break;
        case kRendering:
#ifdef HAVE_LIBSDL
            if ((gDisplay != NULL) && (gDisplayMode == DISP_MODE_SDL))
            {
                fprintf (stderr, "==== [Rendering... Press p to pause, q to quit] ============================\n");
            }
            else
            {
                fprintf (stderr, "==== [Rendering...] ========================================================\n");
            }
#else
            fprintf (stderr, "==== [Rendering...] ========================================================\n");
#endif
            break;
        case kPausedRendering:
#ifdef HAVE_LIBSDL
            if ((gDisplay != NULL) && (gDisplayMode == DISP_MODE_SDL))
            {
                fprintf (stderr, "==== [Paused... Press p to resume] =========================================\n");
            }
            else
            {
                fprintf (stderr, "==== [Paused...] ===========================================================\n");
            }
#else
            fprintf (stderr, "==== [Paused...] ===========================================================\n");
#endif
            break;
    }
}

void CancelRender(websocketpp::connection_hdl hdl, vfeWebsocketSession* session)
{
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
    session->Shutdown();
    delete session;
    session = NULL;
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

void WsHandler::Render(websocketpp::connection_hdl hdl, const string& data)
{
	int argc;
	char** argv;
	ParseCommandLine(data, argc, argv);
	cerr << "chdir: " << argv[0] << endl;
	int ret = chdir(argv[0]);
	if (ret) {
		stringstream ss;
		ss << "Failed to chdir to '" << argv[0] << "'" << endl;
		wsSend(hdl, ss.str());
		return;
	}
	opts = new vfeRenderOptions();
    session = new vfeWebsocketSession();
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
    if (nthreads < 2)
        nthreads = 4;
    opts->SetThreadCount(nthreads);
    session->GetUnixOptions()->ProcessOptions(&argc, &argv);
    session->GetUnixOptions()->Process_povray_ini(*opts);
    while (*++argv)
        opts->AddCommand (*argv);

    if (session->SetOptions(*opts) != vfeNoError)
    {
    	string s = s_stream_fatal;
    	s += "Problem with option setting";
    	wsSend(hdl, s);
//        for(int loony=0;loony<argc_copy;loony++)
//        {
//            fprintf(stderr,"%s%c",argv_copy[loony],loony+1<argc_copy?' ':'\n');
//        }
        ErrorExit(hdl);
        return;
    }
    if (session->StartRender() != vfeNoError) {
        ErrorExit(hdl);
        return;
    }
    //renderMonThread = new boost::thread(RenderMonitor, hdl, session);
    RenderMonitor(hdl, session);
}

void RenderMonitor(websocketpp::connection_hdl hdl, vfeWebsocketSession*& sessionp)
{
	vfeWebsocketSession* session = sessionp;
    // main render loop
    session->SetEventMask(stBackendStateChanged);  // immediatly notify this event
    vfeStatusFlags    flags;
    while (((flags = session->GetStatus(true, 200)) & stRenderShutdown) == 0)
    {
        ProcessSignal();
        if (gCancelRender)
        {
            CancelRender(hdl, session);
            break;
        }

        if (flags & stAnimationStatus)
            fprintf(stderr, "\nRendering frame %d of %d (#%d)\n", session->GetCurrentFrame(), session->GetTotalFrames(), session->GetCurrentFrameId());
        if (flags & stAnyMessage)
            PrintStatus (hdl, session);
        if (flags & stBackendStateChanged) {
           PrintStatusChanged (hdl, session);
        	//fprintf(stderr, "\nUnhandled PrintStatusChanged\n");
        }

//        if (GetRenderWindow() != NULL)
//        {
//            // early exit
//            if (GetRenderWindow()->HandleEvents())
//            {
//                gCancelRender = true;  // will set proper return value
//                CancelRender(session);
//                break;
//            }
//
//            GetRenderWindow()->UpdateScreen();
//
//            // inter-frame pause
//            if (session->GetCurrentFrame() < session->GetTotalFrames()
//            && session->GetPauseWhenDone()
//            && (flags & stAnimationFrameCompleted) != 0
//            && session->Failed() == false)
//            {
//                PauseWhenDone(session);
//                if (! gCancelRender)
//                    session->Resume();
//            }
//        }
    }

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

}

}} /* namespace websockets, namespace povray */