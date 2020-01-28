/******************************************************************************
 * povdbgobject.h - A json object from the POV-Ray symbol table
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
#ifndef POVDBGOBJECT_H
#define POVDBGOBJECT_H

#include <QJsonObject>

namespace pov_parser
{

class Parser;

class PovDbgObject : public QJsonObject
{
public:
	PovDbgObject();
};

class PovDbgObjectFactory
{
public:
	PovDbgObjectFactory() {}
	void setParser(Parser* parser) { mParser = parser; }

	void	parseDbgObject(PovDbgObject& object, const char* name);
private:
	Parser*		mParser;
};

}
#endif // POVDBGOBJECT_H
