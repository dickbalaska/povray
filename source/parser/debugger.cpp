
#include "scanner.h"

#include "debugger.h"

namespace pov_parser
{

Debugger::Debugger(Parser& parser)
	: mParser(parser)
{
	
}

void Debugger::send(const char* text)
{
	mParser.mMessageFactory.Debugger(text);
}

void Debugger::parserPaused()
{
	mParserTask->Pause();
	mParserTask->Cooperate();
}

void Debugger::checkForBreakpoint(const RawToken& rawToken)
{
	POV_LONG col = rawToken.lexeme.position.column;
	if (col == 1) {
		
	}
}

}
