#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "rawtokenizer.h"
#include "parser.h"
#include "backend/control/parsertask.h"

namespace pov_parser
{

class Debugger
{
public:
	Debugger(Parser& parser);
	
	void send(const char* text);
	void parserPaused();
	void checkForBreakpoint(const RawToken& rawToken);
	void setParserTask(ParserTask* task) { mParserTask = task; }
	
private:
	Parser&	mParser;
	ParserTask* mParserTask;
};


}	// namespace pov_parser

#endif // DEBUGGER_H
