/*
 * BerzerkCtl.cpp
 *
 *  Created on: Oct 11, 2017
 *      Author: dick
 */

#include "stdafx.h"
#include "testclient.h"

#include <iostream>
using namespace std;
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string/join.hpp>

using namespace boost;
namespace po = boost::program_options;

namespace testpovws {
TestPovWS	testpovws;
int	DEBUG = 0;
int	_logLevel = 1;
int	_serverPort = -1;

TestPovWS::TestPovWS() {
	client = NULL;

}

TestPovWS::~TestPovWS() {
	// TODO Auto-generated destructor stub
}

int TestPovWS::parseCommandLine(int argc, _TCHAR* argv[]) {
	po::options_description desc("Supported options");
	desc.add_options()
		("help,h", "Display help message")
		("debug,d", po::value<int>(&testpovws::DEBUG)->default_value(0), "debug level 0-9")
		//("l", po::value<int>(&berzerk::_logLevel)->default_value(1), "log level 0-9")
		("port,P", po::value<int>(&testpovws::_serverPort)->default_value(-1), "port that berzerk is listening on")
		(",c", "output the commands to be digested by the completion script")
		("command", po::value< vector<string> >(), "command")
	;
	po::positional_options_description p;
	p.add("command", -1);
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::store(po::command_line_parser(argc, argv). options(desc).positional(p).run(), vm);
	po::notify(vm);
	if (vm.count("help")) {
		//usage();
	    cout << desc << "\n";
	    return (1);
	}
	if (!vm["command"].empty()) {
		BOOST_FOREACH(string token, vm["command"].as<vector<string> >()) {
			command.push_back(token);
		}
	}
	if (command.empty()) {
		command.push_back("help");
		return(0);
	}
	if (DEBUG > 0) {
		cout << "command: ";
		for (list<string>::iterator it=command.begin(); it != command.end(); ++it) {
			cout << ' ' << *it;
		}
		cout << endl;
	}
	return(0);
}
int TestPovWS::execute(int argc, _TCHAR* argv[]) {
	int ret = parseCommandLine(argc, argv);
	if (ret != 0)
		exit(ret);
	client = new Client();
	client->init();
	string s = boost::join(command, " ");
	client->sendCommand(s.c_str());
	while (true) {
		if (!client->waitForReceive()) {

			cout << "received: ";
			client->printReceived();
			client->clearReceive();
			//delete client;
			//return(1);
		}
	}
	delete client;
	return(0);
}
}	// namespace testpovws

int _tmain(int argc, _TCHAR* argv[]) {
	cout << "TestPovWS:" << endl;
	return(testpovws::testpovws.execute(argc, argv));
}

