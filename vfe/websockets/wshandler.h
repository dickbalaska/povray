/*
 * wshandler.h
 *
 *  Created on: Nov 13, 2017
 *      Author: dick
 */

#ifndef VFE_WEBSOCKETS_WSHANDLER_H_
#define VFE_WEBSOCKETS_WSHANDLER_H_

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "wsserver.h"
using namespace std;

namespace povray {
namespace websockets {

extern void waitForShutdown();
extern void notifyShutdown();

typedef string (*commandFunc)(websocketpp::connection_hdl hdl);

class WsHandler {
public:
	WsHandler();
	virtual ~WsHandler();

	static void setHandler(MessageHandlerFunc func);
	void init();
	void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

	static void staticReceiveHandler(websocketpp::connection_hdl hdl, message_ptr msg);
};

}} /* namespace websockets, namespace povray */

#endif /* VFE_WEBSOCKETS_WSHANDLER_H_ */
