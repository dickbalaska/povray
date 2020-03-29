/******************************************************************************
 * povdbgobject.cpp - A json object from the POV-Ray symbol table
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
#include <QJsonArray>

#include "parser.h"

#include "povdbgobject.h"
#include "povdbgobjectnames.h"

const char* s_name("name");
const char* s_type("type");
const char* s_typeS("typeS");
const char* s_value("value");
const char* s_attrs("attrs");

const char* s_Location("Location");
const char* s_Direction("Direction");
const char* s_Up("Up");
const char* s_Right("Right");
const char* s_Sky("Sky");
const char* s_Look_At("Look_At");
const char* s_Focal_Point("Focal_Point");
const char* s_Type("Type");
const char* s_Angle("Angle");

namespace pov_parser
{

using POV_ARRAY = Parser::POV_ARRAY;

static QJsonValue getAttribute(const char* name, const Vector3d& vector);
static QJsonValue getAttribute(const char* name, DBL dbl);

PovDbgObject::PovDbgObject()
{
}

void	PovDbgObjectFactory::parseDbgObject(PovDbgObject& obj, const char* name)
{
	SYM_ENTRY* se = mParser->mSymbolStack.Find_Symbol(name);
	obj[s_name] = name;
	if (!se) {
		obj[s_typeS] = "--";
		obj[s_value] = "<undefined>";
		return;
	}
	TokenId type = GetCategorizedTokenId(se->Token_Number);
	QJsonObject jo = evaluateObject(name, type, se->Data);
	QJsonObject* obj1 = &obj;
	*obj1 = jo;
}

QJsonObject PovDbgObjectFactory::evaluateObject(const char* name, TokenId tid, void* data)
{
	QJsonObject obj;
	obj[s_name] = name;
	TokenId type = GetCategorizedTokenId(tid);
	QString typeString = mParser->Get_Token_String(tid);
	if (typeString.endsWith("identifier"))
		typeString = typeString.left(typeString.length()-11);
	obj[s_typeS] = typeString;
	switch (type) {
	case FLOAT_TOKEN_CATEGORY: {
		DBL* pd = reinterpret_cast<DBL *>(data);
		obj[s_value] = QString("%1").arg(*pd);
		break;
	}
	case VECTOR_TOKEN_CATEGORY: {
		Vector3d* pv = reinterpret_cast<Vector3d*>(data);
		obj[s_value] = QString("%1, %2, %3").arg(pv->x()).arg(pv->y()).arg(pv->z());
		break;
	}
	case COLOUR_ID_TOKEN: {
		RGBFTColour* rgbft = reinterpret_cast<RGBFTColour *>(data);
		obj[s_value] = QString("%1, %2, %3,  %4, %5")
				.arg((double)rgbft->red()).arg((double)rgbft->green()).arg((double)rgbft->blue())
				.arg((double)rgbft->filter()).arg((double)rgbft->transm());
		break;
	}
	case ARRAY_ID_TOKEN: {
		POV_ARRAY* a = reinterpret_cast<POV_ARRAY *>(data);
		QString s;
		for (int i=0; i<=a->maxDim; i++) {
			s += QString("[%1]").arg(a->Sizes[i]);
		}
		obj[s_value] = s;
		if (!a->IsInitialized())
			break;
		QJsonArray ja;
		int ic[POV_ARRAY::kMaxDimensions];
		if (a->maxDim == 0) {
			ic[0] = 0;
			QJsonObject qjo = processArrayDimension(a, ic, 0);
			ja = qjo[s_attrs].toArray();
		} else {
			for (int i=0; i<a->maxDim; i++) {
				ic[0] = i;
				ja.append(processArrayDimension(a, ic, 1));
			}
		}
//		for (int i=0; i<a->maxDim; i++) {
//			for (ic[i]=0; ic[i]<a->Sizes[i]; ic[i]++) {
//				s.clear();
//				int k=0;
//				for (int j=0; j<=i; j++) {
//					s += QString("[%1]").arg(ic[j]);
//					k += a->Sizes[j] * a->Mags[j];
//				}
//				QJsonObject jo;
//				jo[s_name] = s;
//				ja.append(jo);
//				QJsonArray aja;
////				for (int j=0; j<=; j++) {
////					aja.append()
////				}
//			}
//		}
		obj[s_attrs] = ja;
		break;
	}
	case CAMERA_ID_TOKEN: {
		//qDebug() << "Got camera";
		Camera* cam = reinterpret_cast<Camera*>(data);
		QJsonArray ja;
		ja.append(getAttribute(s_Location, cam->Location));
		ja.append(getAttribute(s_Direction, cam->Direction));
		ja.append(getAttribute(s_Up, cam->Up));
		ja.append(getAttribute(s_Right, cam->Right));
		ja.append(getAttribute(s_Sky, cam->Sky));
		ja.append(getAttribute(s_Look_At, cam->Look_At));
		ja.append(getAttribute(s_Focal_Point, cam->Focal_Point));
		ja.append(getAttribute(s_Type, cam->Type));
		ja.append(getAttribute(s_Angle, cam->Angle));
		obj[s_attrs] = ja;
		break;
	}
	case OBJECT_ID_TOKEN: {
		qDebug() << "Got Object";
		break;
	}
	default:
		obj[s_value] = "<unhandled type>";
		break;
	}
	return(obj);
}

QJsonObject PovDbgObjectFactory::processArrayDimension(void* v, int* ic, int dimension)
{
	QJsonObject obj;
	POV_ARRAY* a = reinterpret_cast<POV_ARRAY*>(v);
	if (a->maxDim == dimension) {
		QJsonArray ja;
		QString s;
		for (int i=0; i<dimension; i++) {
			s += QString("[%1]").arg(ic[i]);
		}
		obj[s_name] = s;
		int k = 0;
		for (int i=0; i<dimension; i++) {
			k += ic[i] * a->Mags[i];
		}
		for (int i=0; i<a->Sizes[dimension]; i++) {
			TokenId tid = a->ElementType(i);
			void* v = a->DataPtrs.at(k+i);
			s = QString("[%1]").arg(i);
			ja.append(evaluateObject(s.toStdString().c_str(), tid, v));
		}
		obj[s_attrs] = ja;
		
	} else {
		QString s;
		for (int i=0; i<dimension; i++) {
			s += QString("[%1]").arg(ic[i]);
		}
		obj[s_value] = s;
		QJsonArray ja;
		for (int i=0; i<a->Sizes[dimension]; i++) {
			ic[dimension] = i;
			ja.append(processArrayDimension(v, ic, dimension+1));
		}
		obj[s_attrs] = ja;
	}
	return(obj);
}

static QJsonValue getAttribute(const char* name, const Vector3d& vector)
{
	QJsonObject obj;
	obj[s_name] = name;
	obj[s_typeS] = "Vector3d";
	obj[s_value] = QString("%1, %2, %3").arg(vector.x()).arg(vector.y()).arg(vector.z());
	return(obj);
}

static QJsonValue getAttribute(const char* name, DBL dbl)
{
	QJsonObject obj;
	obj[s_name] = name;
	obj[s_typeS] = "float";
	obj[s_value] = QString("%1").arg(dbl);
	return(obj);
}

}	// namespace pov_parser
