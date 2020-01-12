/******************************************************************************
 * debugger.cpp - The POV-Ray parser filter object
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2020 - Dick Balaska, and BuckoSoft.
 *
 * qtpovray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpovray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
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
