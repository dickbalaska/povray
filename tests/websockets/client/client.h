/*
 * Client.h
 *
 *  Created on: Oct 12, 2017
 *      Author: dick
 */

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "stdafx.h"
#include <string>
#include <queue>
#include <boost/thread/mutex.hpp>

//#include <boost/asio.hpp>

using namespace std;
//using boost::asio::ip::tcp;

namespace testpovws {
class Client {
public:
	Client();
	virtual ~Client();

	bool	init();
	bool	sendCommand(PCSTR command);

	bool	isError() { return(error.empty()); }
	string	getError() { return(error); }
	bool	waitForReceive();
	void	printReceived();

	void	clearReceive();
	std::queue<std::string>& getMessages() { return(messageQueue); }
private:
	string	error;
	string	url;
	int		connId;
//	string	hostname;
//	int		port;
	boost::mutex		messageMutex;
    std::queue<std::string> messageQueue;


};

}
#endif /* _CLIENT_H_ */
