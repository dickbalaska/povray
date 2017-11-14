/*
 * BerzerkCtl.h
 *
 *  Created on: Oct 11, 2017
 *      Author: dick
 */

#ifndef _BERZERKCTL_H_
#define _BERZERKCTL_H_
#include "stdafx.h"

#include <string>
#include <list>
using namespace std;

#include "client.h"

extern int DEBUG;

namespace testpovws {
class TestPovWS {
public:
	TestPovWS();
	virtual ~TestPovWS();

	int	execute(int argc, _TCHAR* argv[]);

private:
	int parseCommandLine(int argc, _TCHAR* argv[]);

	list<string>	command;
	Client*			client;
};
}

#endif /* _BERZERKCTL_H_ */
