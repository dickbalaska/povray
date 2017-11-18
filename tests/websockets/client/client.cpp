/*
 * Client.cpp
 *
 *  Created on: Oct 12, 2017
 *      Author: dick
 */
#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <mutex>
#include <condition_variable>

#include <boost/thread/lock_guard.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/common/memory.hpp>

#include "client.h"

std::mutex mtx;
std::condition_variable mtx_cv;

namespace testpovws {

extern int DEBUG;

bool transactionComplete = false;
bool isTransactionComplete() { return(transactionComplete); }

typedef websocketpp::client<websocketpp::config::asio_client> client;

class connection_metadata {
public:
    typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;
    connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri)
      : m_id(id)
      , m_hdl(hdl)
      , m_status("Connecting")
      , m_uri(uri)
      , m_server("N/A")
    {}
    void on_open(client * c, websocketpp::connection_hdl hdl) {
        m_status = "Open";
        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
        if (DEBUG > 5)
        	cout << "on_open " << m_server << endl;
        if (!m_sendMessages.empty()) {
        	string s = m_sendMessages.front();
        	m_sendMessages.erase(m_sendMessages.begin());
            if (DEBUG > 5)
            	cout << "sending: " << s << endl;
        	con->send(s,websocketpp::frame::opcode::text);
        }
    }
    void on_close(client * c, websocketpp::connection_hdl hdl) {
        m_status = "Closed";
        client::connection_ptr con = c->get_con_from_hdl(hdl);
        std::stringstream s;
        s << "close code: " << con->get_remote_close_code() << " ("
          << websocketpp::close::status::get_string(con->get_remote_close_code())
          << "), close reason: " << con->get_remote_close_reason();
        m_error_reason = s.str();
        if (DEBUG > 6)
        	cout << "on_close " << m_error_reason << endl;
        notifyReceive();
    }
    void on_fail(client * c, websocketpp::connection_hdl hdl) {
        m_status = "Failed";
        client::connection_ptr con = c->get_con_from_hdl(hdl);
        m_server = con->get_response_header("Server");
        m_error_reason = con->get_ec().message();
        std::cerr << "Failed: " << m_error_reason << endl;
        notifyReceive();
    }
    void on_message(websocketpp::connection_hdl hdl, client::message_ptr msg) {
        if (DEBUG > 5)
        	cout << "on_message: ";
        cout << msg->get_payload();
        if (DEBUG > 5)
        	cout << endl;
    	boost::lock_guard<boost::mutex> lock(receiveQueueMutex);
        if (msg->get_opcode() == websocketpp::frame::opcode::text) {
            m_messages.push_back(string(msg->get_payload()));
        } else {
            m_messages.push_back(websocketpp::utility::to_hex(msg->get_payload()));
        }
        notifyReceive();
    }
    void notifyReceive() {
        std::unique_lock<std::mutex> lck(mtx);
		transactionComplete = true;
		mtx_cv.notify_one();
    }
    friend std::ostream & operator<< (std::ostream & out, connection_metadata const & data);
    string get_status() { return(m_status); }
    int		get_id() { return(m_id); }
    websocketpp::connection_hdl get_hdl() { return(m_hdl); }
	boost::mutex		receiveQueueMutex;
    std::vector<std::string> m_messages;
    std::vector<string> m_sendMessages;

    void queueSend(string& message) {
    	m_sendMessages.push_back(message);
    }
private:
    int m_id;
    websocketpp::connection_hdl m_hdl;
    std::string m_status;
    std::string m_uri;
    std::string m_server;
    std::string m_error_reason;
};
std::ostream & operator<< (std::ostream & out, connection_metadata const & data) {
    out << "> URI: " << data.m_uri << "\n"
        << "> Status: " << data.m_status << "\n"
        << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
        << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);
    return out;
}
class websocket_endpoint {
public:
	websocket_endpoint () : m_next_id(0) {
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);
		m_endpoint.init_asio();
		m_endpoint.start_perpetual();
		m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
	}
	~websocket_endpoint() {
		m_endpoint.stop_perpetual();
		for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
			if (it->second->get_status() != "Open") {
				// Only close open connections
				continue;
			}
	        if (DEBUG > 6)
	        	std::cout << "> Closing connection " << it->second->get_id() << std::endl;
			websocketpp::lib::error_code ec;
			m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
			if (ec) {
				std::cerr << "> Error closing connection " << it->second->get_id() << ": "
						<< ec.message() << std::endl;
			}
		}
		m_thread->join();
	}
	/** connect to a remote socket
	 * @return the id of the connection. -1 on failure
	 */
	int connect(std::string const & url) {
		websocketpp::lib::error_code ec;
		client::connection_ptr con = m_endpoint.get_connection(url, ec);
		if (ec) {
			std::cerr << "> Connect initialization error: " << ec.message() << std::endl;
			transactionComplete = true;
			return -1;
		}
		int new_id = m_next_id++;
		connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), url));
		m_connection_list[new_id] = metadata_ptr;
		con->set_open_handler(websocketpp::lib::bind(
				&connection_metadata::on_open,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
		));
		con->set_fail_handler(websocketpp::lib::bind(
				&connection_metadata::on_fail,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
		));
		con->set_message_handler(websocketpp::lib::bind(
		    &connection_metadata::on_message,
		    metadata_ptr,
		    websocketpp::lib::placeholders::_1,
		    websocketpp::lib::placeholders::_2
		));
		m_endpoint.connect(con);
		return new_id;
	}
	void close(int id, websocketpp::close::status::value code) {
		websocketpp::lib::error_code ec;
		con_list::iterator metadata_it = m_connection_list.find(id);
		if (metadata_it == m_connection_list.end()) {
			std::cerr << "> No connection found with id " << id << std::endl;
			transactionComplete = true;
			return;
		}
		m_endpoint.close(metadata_it->second->get_hdl(), code, "", ec);
		if (ec) {
			std::cerr << "> Error initiating close: " << ec.message() << std::endl;
		}
	}
	void send(int id, std::string message) {
	    websocketpp::lib::error_code ec;
	    con_list::iterator metadata_it = m_connection_list.find(id);
	    if (metadata_it == m_connection_list.end()) {
	        std::cerr << "> No connection found with id " << id << std::endl;
	        return;
	    }
	    if (metadata_it->second->get_status() != "Open") {
	    	metadata_it->second->queueSend(message);
	    	return;
	    }
        if (DEBUG > 5)
        	cout << "sending message: '" << message << "'" << endl;
	    m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
	    if (ec) {
	        cerr << "> Error sending message: " << ec.message() << std::endl;
	        return;
	    }
	   // metadata_it->second->record_sent_message(message);
	}
	connection_metadata::ptr get_metadata(int id) const {
		con_list::const_iterator metadata_it = m_connection_list.find(id);
		if (metadata_it == m_connection_list.end()) {
			return connection_metadata::ptr();
		} else {
			return metadata_it->second;
		}
	}
private:
	typedef std::map<int,connection_metadata::ptr> con_list;
	client m_endpoint;
	websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
	con_list m_connection_list;
	int m_next_id;
};

websocket_endpoint endpoint;

////////////////////////////////////////////////////////////////////////////////
Client::Client() {
//	 hostname = "localhost";
//	 port = 9090;
	 url = "ws://localhost:4401";
//	 url = "ws://echo.websocket.org";
	 connId = -1;
	 error.clear();
}

Client::~Client() {
}

void Client::clearReceive() {
	transactionComplete = false;
}
bool Client::init() {
	try {
		connId = endpoint.connect(url);
		if (connId < 0) {
			return(false);
		}
	} catch (std::exception& e) {
		error = "socket init error: ";
		error += e.what();
		return(false);
	}
	return(true);
}
bool Client::sendCommand(PCSTR command) {
	try {
		endpoint.send(connId, command);
	} catch (std::exception& e) {
		error = e.what();
		return(false);
	}
	return(true);
}

bool Client::waitForReceive() {
	if (!isTransactionComplete()) {
		std::unique_lock<mutex> lck(mtx);
		mtx_cv.wait(lck, isTransactionComplete);
	}
	boost::lock_guard<boost::mutex> lock(messageMutex);
	boost::lock_guard<boost::mutex> rqlock(endpoint.get_metadata(connId)->receiveQueueMutex);
	for (vector<string>::iterator it=endpoint.get_metadata(connId)->m_messages.begin(); it != endpoint.get_metadata(connId)->m_messages.end(); ++it) {
		string s = *it;
		messageQueue.push(s);
        //if (DEBUG <= 5)
        //	cout << *it << endl;
	}
	endpoint.get_metadata(connId)->m_messages.clear();
	//m_messages = endpoint.get_metadata(connId)->m_messages;
	return(true);
}

void Client::printReceived() {
	boost::lock_guard<boost::mutex> lock(messageMutex);
	while (!messageQueue.empty()) {
		string s = messageQueue.front();
		cout << s;
		messageQueue.pop();
	}
//	for (vector<string>::iterator it=getMessages().begin(); it != client->getMessages().end(); ++it) {
//		cout << ' ' << *it;
//	}
	cout << endl;

}
}	// namespace berzerk
