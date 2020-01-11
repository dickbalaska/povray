#include <QDebug>
#include "scanner.h"

#include "debugger.h"

namespace pov_parser
{

Debugger::Debugger()
{
	
}

void Debugger::send(const char* text)
{
	mParser->mMessageFactory.Debugger(text);
}

void Debugger::debuggerPaused()
{
	mParserTask->PauseForDebugger();
	mParserTask->DebuggerPaused();
}

void Debugger::checkForBreakpoint(const RawToken& rawToken)
{
	POV_LONG col = rawToken.lexeme.position.column;
	if (col == 1) {
		
	}
}

void Debugger::messageFromGui(const std::string& msg)
{
	qDebug() << "Debugger:messageFromGui";
	mParserTask->ResumeFromDebugger();
}

}
