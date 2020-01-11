#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "rawtokenizer.h"
#include "backend/control/parsertask.h"
#include "parser.h"

namespace pov_parser
{

class Debugger
{
public:
	Debugger();
	
	void send(const char* text);
	void debuggerPaused();
	void checkForBreakpoint(const RawToken& rawToken);
	void setParser(Parser* parser) { mParser = parser; }
	void setParserTask(ParserTask* task) { mParserTask = task; }
	
	void messageFromGui(const std::string& msg);

private:
	Parser*	mParser;
	ParserTask* mParserTask;
};


}	// namespace pov_parser

#endif // DEBUGGER_H
