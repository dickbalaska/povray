//******************************************************************************
///
/// @file vfe/websockets/wsserver.cpp
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

#include "wsserver.h"

#include <map>
#include "boost/thread/shared_mutex.hpp"

#include "websocketpp/server.hpp"
#include "websocketpp/config/asio_no_tls.hpp"



// static initialisations

// namespace merging
using websocketpp::connection_hdl;

extern bool gCancelRender;
bool gDeadConnection = false;

namespace povray {
namespace websockets {
int DEBUG = 9;

websocketpp::server<websocketpp::config::asio> WebsocketServer::server;
map<string, connection_hdl> WebsocketServer::websockets;

LogStream WebsocketServer::ls;
ostream WebsocketServer::os(&ls);

MessageHandlerFunc messageHandlerFunc = NULL;

//pthread_rwlock_t WebsocketServer::websocketsLock = PTHREAD_RWLOCK_INITIALIZER;
typedef boost::shared_mutex BMutex;
typedef boost::unique_lock<BMutex> WriteLock;
typedef boost::shared_lock<BMutex> ReadLock;
BMutex mtx_;

void WebsocketServer::init()
{
	// Initialising WebsocketServer.
	server.init_asio();
	// Set custom logger (ostream-based).
	server.get_alog().set_ostream(&os);
	server.get_elog().set_ostream(&os);
	// Register the message handlers.
	//server.set_validate_handler(&WebsocketServer::on_validate);
	server.set_message_handler(&WebsocketServer::on_message);
	server.set_fail_handler(&WebsocketServer::on_fail);
	server.set_close_handler(&WebsocketServer::on_close);

}
bool WebsocketServer::listen(int port) {
	// Listen on port.
	//int port = 4441;
	try {
		//server.listen(port);
        server.listen(websocketpp::lib::asio::ip::tcp::v4(), port);
	} catch(websocketpp::exception const &e) {
		// Websocket exception on listen. Get char string via e.what().
		return(false);
	}
	// Starting Websocket accept.
	websocketpp::lib::error_code ec;
	server.start_accept(ec);
	if (ec) {
		// Can log an error message with the contents of ec.message() here.
		return false;
	}

	return true;
}

void WebsocketServer::setReceiveHandler(MessageHandlerFunc func) {
	messageHandlerFunc = func;
}

void WebsocketServer::run() {
	try {
		server.run();
	} catch(websocketpp::exception const &e) {
		// Websocket exception. Get message via e.what().
	}
}

void WebsocketServer::stop() {
	// Stopping the Websocket listener and closing outstanding connections.
	websocketpp::lib::error_code ec;
	server.stop_listening(ec);
	if (ec) {
		// Failed to stop listening. Log reason using ec.message().
		return;
	}

	// Close all existing websocket connections.
	string data = "Terminating connection...";
	map<string, connection_hdl>::iterator it;
	for (it = websockets.begin(); it != websockets.end(); ++it) {
		websocketpp::lib::error_code ec;
		server.close(it->second, websocketpp::close::status::normal, data, ec); // send text message.
		if (ec) { // we got an error
			// Error closing websocket. Log reason using ec.message().
		}
	}

	// Stop the endpoint.
	server.stop();
}

//bool WebsocketServer::on_validate(connection_hdl hdl) {
//	websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
//	websocketpp::uri_ptr uri = con->get_uri();
//	string query = uri->get_query(); // returns empty string if no query string set.
//	string id = "0";
//	websocketpp::connection<websocketpp::config::asio>::message_ptr mp = con->get_message(websocketpp::frame::opcode::text, 512);
//	string s = mp->get_payload();
//	if (!query.empty()) {
//		// Split the query parameter string here, if desired.
//		// We assume we extracted a string called 'id' here.
//	}
//	else {
//		// Reject if no query parameter provided, for example.
////		return false;
//	}
//
//	if (pthread_rwlock_wrlock(&websocketsLock) != 0) {
//		// Failed to write-lock websocketsLock.
//	}
//
//	websockets.insert(std::pair<string, connection_hdl>(id, hdl));
//	if (pthread_rwlock_unlock(&websocketsLock) != 0) {
//		// Failed to unlock websocketsLock.
//	}
//	websocketpp::lib::error_code ec;
//	server.send(hdl, s, websocketpp::frame::opcode::text, ec); // send text message.
//	cerr << "WebsocketServer::send: " << ec.message() << endl;
//	return true;
//}

void WebsocketServer::on_message(websocketpp::connection_hdl hdl, message_ptr msg) {
#ifdef _DEBUG
	if (DEBUG)
		std::cerr << "on_message called with hdl: " << hdl.lock().get()
				  << " and message: " << msg->get_payload()
				  << endl;
#endif
	gDeadConnection = false;
    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
//    if (msg->get_payload() == "stop-listening") {
//    	server.stop_listening();
//        return;
//    }
    if (messageHandlerFunc != NULL)
    	messageHandlerFunc(hdl, msg);
//    try {
//    	server.send(hdl, msg->get_payload(), msg->get_opcode());
//    } catch (const websocketpp::lib::error_code& e) {
//        std::cout << "Echo failed because: " << e
//                  << "(" << e.message() << ")" << std::endl;
//    }
}

bool WebsocketServer::getWebsocket(const string &id, websocketpp::connection_hdl &hdl) {
	map<string, connection_hdl>::iterator iter;
	bool found = false;
	//pthread_rwlock_rdlock(&websocketsLock);
	ReadLock r_lock(mtx_);
	iter = websockets.find(id);
	if (iter != websockets.end()) {
		hdl = iter->second;
		found = true;
	}
	//pthread_rwlock_unlock(&websocketsLock);
	r_lock.unlock();

	return(found);
}

void WebsocketServer::on_fail(connection_hdl hdl) {
	websocketpp::server<websocketpp::config::asio>::connection_ptr con = server.get_con_from_hdl(hdl);
	websocketpp::lib::error_code ec = con->get_ec();
	// Websocket connection attempt by client failed. Log reason using ec.message().
	cerr << "WebsocketServer::on_fail: " << ec.message() << endl;
}

void WebsocketServer::on_close(connection_hdl hdl) {
	// Websocket connection closed.
#ifdef _DEBUG
	cerr << "WebsocketServer::on_close" << endl;
#endif
}
void WebsocketServer::send(websocketpp::connection_hdl hdl, const string& msg) {
	if (gDeadConnection)
		return;
	websocketpp::lib::error_code ec;
	server.send(hdl, msg, websocketpp::frame::opcode::text, ec);
	if (ec) {
		cerr << "Error sending: " << ec.message() << endl;
		gCancelRender = true;
		gDeadConnection = true;
	}
}

//bool WebsocketServer::sendData(string id, string data) {
//	connection_hdl hdl;
//	if (!getWebsocket(id, hdl)) {
//		// Sending to non-existing websocket failed.
//		return false;
//	}
//
//	websocketpp::lib::error_code ec;
//	server.send(hdl, data, websocketpp::frame::opcode::text, ec); // send text message.
//	if (ec) { // we got an error
//		cerr << "Error sendDataing: " << ec.message() << endl;
//		return false;
//	}
//
//	return true;
//}

bool WebsocketServer::sendBinary(connection_hdl hdl, const char* data, int size) {
	if (gDeadConnection)
		return(false);
	websocketpp::lib::error_code ec;
	server.send(hdl, data, size, websocketpp::frame::opcode::binary, ec);
	if (ec) {
		cerr << "Error sending binary: " << ec.message() << endl;
		gCancelRender = true;
		gDeadConnection = true;
		return(false);
	}

	return(true);
}

bool WebsocketServer::sendClose(string id) {
	connection_hdl hdl;
	if (!getWebsocket(id, hdl)) {
		// Closing non-existing websocket failed.
		return false;
	}

	string data = "Terminating connection...";
	websocketpp::lib::error_code ec;
	server.close(hdl, websocketpp::close::status::normal, data, ec); // send close message.
	if (ec) { // we got an error
		// Error closing websocket. Log reason using ec.message().
		return false;
	}

	// Remove websocket from the map.
	//pthread_rwlock_rdlock(&websocketsLock);
	WriteLock w_lock(mtx_);
	websockets.erase(id);
	//pthread_rwlock_unlock(&websocketsLock);
	w_lock.unlock();

	return true;
}


}} /* namespace websockets, namespace povray */