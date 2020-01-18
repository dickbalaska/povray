/******************************************************************************
 * debuggermessages.cpp - Define the message interface between the gui and the parser
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

#include "debuggermessages.h"

// commands from gui
const char* s_cont("cont");		// continue
const char* s_b("b");			// set breakpoint
const char* s_pause("pause");	// break at next available
const char* s_step("step");		// step to next line
const char* s_w("w");			// watch symbol
const char* s_resetBreakpoints("resetBreakpoints");
const char* s_resetWatches("resetWatches");

// commands to gui

const char* s_sym("sym");
const char* s_init("init");
const char* s_break("break");
