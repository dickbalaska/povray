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
#include <vector>
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
	void	clearReceive();
	std::vector<std::string>& getMessages() { return(m_messages); }
private:
	string	error;
	string	url;
	int		connId;
//	string	hostname;
//	int		port;
    std::vector<std::string> m_messages;


};

}
#endif /* _CLIENT_H_ */
