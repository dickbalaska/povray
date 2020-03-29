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
#include <QJsonDocument>
#include <QDir>

#include "base/stringutilities.h"
#include "debuggermessages.h"
#include "scanner.h"

#include "debugger.h"

namespace pov_parser
{

struct Breakpoint
{
	QString	filePath;
	POV_LONG line;
};

// These attributes are not in the class decl solely so that I don't have to include Qt files in the .h
static QList<Breakpoint> breakpoints;
static QList<QString>	 watches;
static QString			 startPath;

Debugger::Debugger()
{
}

void Debugger::init()
{
	breakpoints.clear();
	watches.clear();
	startPath = QDir::currentPath();
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
	QDir f;
	bool doBreak = false;
	//POV_LONG col = rawToken.lexeme.position.column;
	POV_LONG line = rawToken.lexeme.position.line;
	//if (col == 1) {
	QString canonicalPath;
	if (line != mParseLine) {
		//qDebug() << "col1" << line;
		std::string filePath;
		filePath = UCS2toSysString(mParser->mTokenizer.GetInputStreamName().c_str());
		canonicalPath =  QString("%1/%2").arg(startPath, UCS2toSysString(mParser->mTokenizer.GetInputStreamName().c_str()).c_str());
		QDir d(canonicalPath);
		canonicalPath = d.canonicalPath();
		mParseLine = (int)line;
		if (mStepping) {
			doBreak = true;
			mStepping = false;
		} else {
			for (const Breakpoint& bp : breakpoints) {
				if (bp.line == line) {
					if (bp.filePath == canonicalPath)
						doBreak = true;
				}
			}
		}
	}
	if (doBreak) {
		QString s = QString("break %1 %2").arg(line).arg(canonicalPath);
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
		bp.filePath = path;
		breakpoints.append(bp);
		return;
	}
	if (command == s_cont) {
		
		mParserTask->ResumeFromDebugger();
		return;
	}
	if (command == s_step) {
		mStepping = true;
		mParserTask->ResumeFromDebugger();
		return;
	}
	if (command == s_pause) {
		
	}
	if (command == s_resetBreakpoints) {
		breakpoints.clear();
		return;
	}
	if (command == s_resetWatches) {
		watches.clear();
		return;
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
	QJsonDocument doc;
	PovDbgObject  obj;
	mDbgObjectFactory.parseDbgObject(obj, name);
	doc.setObject(obj);
	
/*	QString typeString;
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
	*/
	QString msg = QString("%1 %2").arg(s_sym, doc.toJson().toStdString().c_str());	
	send(msg.toUtf8().toStdString().c_str());
}

}
