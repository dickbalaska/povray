/******************************************************************************
 * debugger.cpp - The qtpovray SDL debugger
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
#include "base/stringutilities.h"
#include "debuggermessages.h"
#include "scanner.h"

#include "debugger.h"

namespace pov_parser
{

struct Breakpoint
{
	std::string	filePath;
	POV_LONG line;
};

static QList<Breakpoint> breakpoints;
static QList<QString>	 watches;

Debugger::Debugger()
{
	
}

void Debugger::init()
{
	breakpoints.clear();
	watches.clear();
	send(s_init);
}

void Debugger::send(const char* text)
{
	mParser->mMessageFactory.Debugger(text);
}

void Debugger::debuggerPaused()
{
	sendWatches();
	mParserTask->PauseForDebugger();
	mParserTask->DebuggerPaused();
}

void Debugger::checkForBreakpoint(const RawToken& rawToken)
{
	bool doBreak = false;
	POV_LONG col = rawToken.lexeme.position.column;
	POV_LONG line = rawToken.lexeme.position.line;
	std::string filePath;
	if (col == 1) {
		qDebug() << "col1" << line;
		
		for (const Breakpoint& bp : breakpoints) {
			if (bp.line == line) {
				if (filePath.empty()) {
					filePath = UCS2toSysString(mParser->mTokenizer.GetInputStreamName().c_str());
					UCS2String s;
				}
				if (bp.filePath == filePath)
					doBreak = true;
			}
		}
	}
	if (doBreak) {
		QString s = QString("break %1 %2").arg(line).arg(filePath.c_str());
		send(s.toUtf8().toStdString().c_str());
		debuggerPaused();	// must be last
	}
}

void Debugger::messageFromGui(const char* msg)
{
	qDebug() << "Debugger:messageFromGui";
	QString s(msg);
	QString command;
	QString data;
	int i = s.indexOf(' ');
	if (i == -1)
		command = s;
	else {
		command = s.left(i);
		data = s.mid(i+1);
	}
	if (command == s_cont) {
		mParserTask->ResumeFromDebugger();
		return;
	}
	if (command == s_b) {
		i = data.indexOf(' ');
		if (i == -1) {
			qCritical() << "Debugger: Bad breakpoint msg" << msg;
			return;
		}
		Breakpoint bp;
		int line = data.left(i).toInt();
		QString path = data.mid(i+1);
		bp.line = line;
		bp.filePath = path.toStdString();
		breakpoints.append(bp);
		return;
	}
	if (command == s_pause) {
		
	}
	if (command == s_w) {
		commandWatchSymbol(data.toUtf8().toStdString().c_str());
	}
}

void Debugger::commandWatchSymbol(const char* name)
{
	watches.append(name);
	sendWatch(name);
}

void Debugger::sendWatches()
{
	for (const QString& s : watches) {
		sendWatch(s.toUtf8().toStdString().c_str());
	}
}
void Debugger::sendWatch(const char* name)
{
	QString typeString;
	QString value;
	SYM_ENTRY* se = mParser->mSymbolStack.Find_Symbol(name);
	if (!se) {
		typeString = "--";
		value = "<undefined>";
	} else {
		TokenId type = GetCategorizedTokenId(se->Token_Number);
		typeString = mParser->Get_Token_String(se->Token_Number);
		if (typeString.endsWith("identifier"))
			typeString = typeString.left(typeString.length()-11);
		switch (type) {
		case FLOAT_TOKEN_CATEGORY: {
			DBL* pd = reinterpret_cast<DBL *>(se->Data);
			value = QString("%1").arg(*pd);
			break;
		}
		case VECTOR_TOKEN_CATEGORY: {
			Vector3d* pv = reinterpret_cast<Vector3d*>(se->Data);
			value = QString("%1,%2,%3").arg(pv->x()).arg(pv->y()).arg(pv->z());
			break;
		}
		default:
			value = "<unhandled type>";
			break;
		}
	}
	QString msg = QString("%1 %2 %3 %4").arg(s_sym, name, typeString, value);	
	send(msg.toUtf8().toStdString().c_str());
}

}
