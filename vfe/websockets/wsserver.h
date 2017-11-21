/*
 * websocketserver.h
 *
 *  Created on: Oct 29, 2017
 *      Author: Dick Balaska
 */

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
	static pthread_rwlock_t websocketsLock;
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
