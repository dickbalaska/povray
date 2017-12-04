//******************************************************************************
///
/// @file vfe/websockets/wsserver.h
///
/// WebSocket server for povrayws
///
/// @author Dick Balaska <dick@buckosoft.com>
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2017 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//*******************************************************************************

#ifndef _WEBSOCKET_WEBSOCKETSERVER_H_
#define _WEBSOCKET_WEBSOCKETSERVER_H_

#include <string>
#include <boost/algorithm/string.hpp>

using namespace std;

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"


namespace povray {
namespace websockets {
typedef websocketpp::server<websocketpp::config::asio> server;
typedef server::message_ptr message_ptr;
//typedef void (*MessageHandlerFunc)(websocketpp::connection_hdl hdl, message_ptr msg);
using MessageHandlerType = void(websocketpp::connection_hdl hdl, message_ptr msg);
using MessageHandlerFunc = MessageHandlerType*;

class LogStream : public streambuf {
private:
    string buffer;

protected:
    int overflow(int ch) override {
        buffer.push_back((char) ch);
        if (ch == '\n') {
            // End of line, write to logging output and clear buffer.

            buffer.clear();
        }

        return ch;

        //  Return traits::eof() for failure.
    }
};


class WebsocketServer {
public:
	static void init();
	static bool listen(int port);
	static void run();
	static void stop();

	static bool sendClose(string id);
	static bool sendData(string id, string data);
	static bool	sendBinary(websocketpp::connection_hdl hdl, const char* data, int size);
	static void setReceiveHandler(MessageHandlerFunc func);
	static void send(websocketpp::connection_hdl hdl, const string& msg);
private:
	static bool getWebsocket(const string &id, websocketpp::connection_hdl &hdl);

	static websocketpp::server<websocketpp::config::asio> server;
	//static pthread_rwlock_t websocketsLock;
	static map<string, websocketpp::connection_hdl> websockets;
	static LogStream ls;
	static ostream os;

	// callbacks
	static void on_message(websocketpp::connection_hdl hdl, message_ptr msg);
	//static bool on_validate(websocketpp::connection_hdl hdl);
	static void on_fail(websocketpp::connection_hdl hdl);
	static void on_close(websocketpp::connection_hdl hdl);

	WebsocketServer() {}
};

}} /* namespace websockets, namespace povray */

#endif /* _WEBSOCKET_WEBSOCKETSERVER_H_ */
