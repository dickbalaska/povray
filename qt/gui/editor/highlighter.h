/******************************************************************************
 * highlighter.cpp - Color the text in the editor
 *
 * qtpov - A Qt IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
 *
 * qtpov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * qtpov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef _HIGHLIGHTER_H_
#define _HIGHLIGHTER_H_

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

#include "../preferencedata.h"

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class CodeEditor;
typedef const char* PCSTR;

struct BracketInfo
{
	BracketInfo() {}
	BracketInfo(QChar _c, int _p) { c = _c; position = _p; }
	QChar c;
	int position;
};

struct BlockData : public QTextBlockUserData
{
	QVector<BracketInfo> brackets;
};

class Highlighter : public QSyntaxHighlighter
{
	Q_OBJECT

public:
	Highlighter(PreferenceData* prefs, QTextDocument *parent = 0);

protected:
	void highlightBlock(const QString &text) override;

private:
	void setupType(QTextCharFormat& format, PCSTR* list, Highlight& highlight);

	struct HighlightingRule
	{
		QRegularExpression pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;

	QRegularExpression commentStartExpression;
	QRegularExpression commentEndExpression;
	QRegularExpression stringMarkerExpression;
	QRegularExpression singleLineCommentExpression;

	QTextCharFormat commonFormat;
	QTextCharFormat mathFormat;
	QTextCharFormat modifierFormat;
	QTextCharFormat objectFormat;
	QTextCharFormat textureFormat;
	QTextCharFormat colorFormat;
	QTextCharFormat singleLineCommentFormat;
	QTextCharFormat multiLineCommentFormat;
	QTextCharFormat quotationFormat;
};

#endif // _HIGHLIGHTER_H_
