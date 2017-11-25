/*
 * wshandler.h
 *
 *  Created on: Nov 13, 2017
 *      Author: dick
 */

#ifndef _VFE_WEBSOCKETS_WSHANDLER_H_
#define _VFE_WEBSOCKETS_WSHANDLER_H_

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "vfe.h"
#include "wsoptions.h"
#include "wsserver.h"

using namespace std;
using namespace vfe;
using namespace vfePlatform;

namespace povray {
namespace websockets {


extern void waitForShutdown();
extern void notifyShutdown();

extern void PrintStatus(websocketpp::connection_hdl hdl, vfeWebsocketSession* session);
extern void PrintNonStatusMessage(websocketpp::connection_hdl hdl, vfeWebsocketSession* session);
extern void CancelRender(websocketpp::connection_hdl hdl, vfeWebsocketSession* session);
extern void RenderMonitor(websocketpp::connection_hdl hdl, vfeWebsocketSession*& sessionp);
extern void wsSend(websocketpp::connection_hdl hdl, const char* msg);
inline void wsSend(websocketpp::connection_hdl hdl, const string& msg) { wsSend(hdl, msg.c_str()); }

typedef string (*commandFunc)(websocketpp::connection_hdl hdl);

class WsHandler {
public:
	WsHandler();
	virtual ~WsHandler();

	static void setHandler(MessageHandlerFunc func);
	void init();
	void on_message(websocketpp::connection_hdl hdl, message_ptr msg);

	static void staticReceiveHandler(websocketpp::connection_hdl hdl, message_ptr msg);

private:

	void Quit(websocketpp::connection_hdl hdl);
	void Cancel(websocketpp::connection_hdl hdl);
//	void CancelRender(websocketpp::connection_hdl hdl);
	void ErrorExit(websocketpp::connection_hdl hdl);
	void ParseCommandLine(const string& data, int& argc, char**& argv);
	void DeleteArgv(char**& argv);
	void PrintVersion(websocketpp::connection_hdl hdl);
	void PrintStatus(websocketpp::connection_hdl hdl);
	void Render(websocketpp::connection_hdl hdl, const string& data);

	vfeWebsocketSession*	session;
    boost::thread*			renderMonThread;

};

}} /* namespace websockets, namespace povray */

#endif /* _VFE_WEBSOCKETS_WSHANDLER_H_ */
