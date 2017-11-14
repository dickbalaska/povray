/*
 * wshandler.cpp
 *
 *  Created on: Nov 13, 2017
 *      Author: dick
 */

#include <mutex>
#include <condition_variable>

#include "wshandler.h"

namespace povray {
namespace websockets {

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
		std::unique_lock<mutex> lck(shutdownMtx);
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

void WsHandler::on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
	string s = msg->get_payload();
}

}} /* namespace websockets, namespace povray */
