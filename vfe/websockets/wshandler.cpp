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

namespace povray {
namespace websockets {

using namespace std;
using namespace boost;

WsHandler websocketHandler;

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

WsHandler::WsHandler() {
	// TODO Auto-generated constructor stub

}

WsHandler::~WsHandler() {
	// TODO Auto-generated destructor stub
}

void WsHandler::send(websocketpp::connection_hdl hdl, const char* msg) {
    try {
    	WebsocketServer::send(hdl, msg);
    } catch (const websocketpp::lib::error_code& e) {
        std::cerr << "send failed because: " << e
                  << "(" << e.message() << ")" << std::endl;
    }
}

void WsHandler::on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
	string s = msg->get_payload();
	tokenizer<> tok(s);
	tokenizer<>::iterator beg=tok.begin();
	string command = *beg;
	cerr << "Command = '" << command << "'" << endl;
	if (command == "version")
		PrintVersion(hdl);
	else if (command == "quit")
		Quit(hdl);
	else {
		s = "unknown command: ";
		s += command;
		send(hdl, s.c_str());
	}
}
void WsHandler::Quit(websocketpp::connection_hdl hdl) {
	cerr << "exit on user command" << endl;
	notifyShutdown();
}

void WsHandler::PrintVersion(websocketpp::connection_hdl hdl) {
	char s[2048];
    snprintf(s, 2048,
        "%s %s\n\n"
        "%s\n%s\n%s\n"
        "%s\n%s\n%s\n\n",
        PACKAGE_NAME, POV_RAY_VERSION,
        DISTRIBUTION_MESSAGE_1, DISTRIBUTION_MESSAGE_2, DISTRIBUTION_MESSAGE_3,
        POV_RAY_COPYRIGHT, DISCLAIMER_MESSAGE_1, DISCLAIMER_MESSAGE_2
    );
	send(hdl, s);
    snprintf(s, 2048,
        "Built-in features:\n"
        "  I/O restrictions:          %s\n"
        "  X Window display:          %s\n"
        "  WebSockets:                %s\n"
        "  Supported image formats:   %s\n"
        "  Unsupported image formats: %s\n\n",
        BUILTIN_IO_RESTRICTIONS, BUILTIN_XWIN_DISPLAY, BUILTIN_WEBSOCKETS, BUILTIN_IMG_FORMATS, MISSING_IMG_FORMATS
    );
	send(hdl, s);
    snprintf(s, 2048,
        "Compilation settings:\n"
        "  Build architecture:  %s\n"
        "  Built/Optimized for: %s\n"
        "  Compiler vendor:     %s\n"
        "  Compiler version:    %s\n"
        "  Compiler flags:      %s\n",
        BUILD_ARCH, BUILT_FOR, COMPILER_VENDOR, COMPILER_VERSION, CXXFLAGS
    );
	send(hdl, s);
}

}} /* namespace websockets, namespace povray */
