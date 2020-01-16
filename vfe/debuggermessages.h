/******************************************************************************
 * debuggermessages.h - Define the message interface between the gui and the parser
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
#include <QString>

// messages from gui to debugger
extern const char* s_cont;		// continue
extern const char* s_b;			// set breakpoint
extern const char* s_pause;		// break at next available
extern const char* s_step;		// step to next line
extern const char* s_w;			// watch symbol


///////////////////////////////////////////////////////////////////////////////
// messages from debugger to gui

/** "init"
 * Sent as soon as the parser starts.
 * Gui responds with wanted breakpoints, then watched symbols, followed by cont
 */
extern const char* s_init;

/** "break lineNumber fileName"
 */
extern const char* s_break;

/** "sym name typeString value"
 * typeString = from Reserved_Words[] minus any trailing " identifier".
 * if typeString == "--" then symbol not found
 * value = convert the supported value types to a string
 * 
 */
extern const char* s_sym;

enum DbgSymbolType
{
	dbgFloat,
	dbgVector,
	dbgUnknown = -1
};
