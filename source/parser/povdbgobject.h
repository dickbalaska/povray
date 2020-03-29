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

#include "reservedwords.h"
#include "parser.h"

namespace pov_parser
{

//class Parser;
//using POV_ARRAY = Parser::POV_ARRAY;

class PovDbgObject : public QJsonObject
{
public:
	PovDbgObject();
	void operator=(const QJsonObject& other) { ((QJsonObject)*this) = other; }
};

class PovDbgObjectFactory
{
public:
	PovDbgObjectFactory() {}
	void setParser(Parser* parser) { mParser = parser; }

	void	parseDbgObject(PovDbgObject& object, const char* name);
private:
	QJsonObject	evaluateObject(const char* name, TokenId type, void* data);
	QJsonObject	processArrayDimension(void* v, int* ic, int dimension);
	Parser*		mParser;
};

}
#endif // POVDBGOBJECT_H
