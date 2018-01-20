/******************************************************************************
 * codeeditor.cpp - Extend QPlainTextEdit with POV-Ray customizations
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

#include "../preferencedata.h"
#include "../dock/maintoolbar.h"
#include "mainwindow.h"
#include "bookmarkman.h"
#include "findman.h"
#include "insertmenuman.h"
#include "coloreditor.h"
#include "colormapeditor.h"
#include "codeeditor.h"

#include <QtWidgets>

QHash<QString, int>	editKeywords;

void CodeEditor::init()
{
	editKeywords["rgb"] = 0;
	editKeywords["rgbf"] = 0;
	editKeywords["rgbt"] = 0;
	editKeywords["rgbft"] = 0;
	editKeywords["color_map"] = 1;
}

CodeEditor::CodeEditor(MainWindow* parent, PreferenceData* prefs)
	: QPlainTextEdit(parent),
	  m_mainWindow(parent),
	  m_prefs(prefs),
	  m_tooltip(NULL),
	  m_tooltipTimer(this),
	  m_backgroundPixmap(80,50)
	  //m_tooltipPixmap(40,40)
{
	modified = false;
	bHighlightCurrentLine = true;
	bHighlightTokens = true;
	m_bookmarkOldBlockCount = 0;
	m_lastTooltipPosition = -1;
	m_hasUndo = false;
	m_hasRedo = false;

	m_lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateHighlights()));
	connect(this, SIGNAL(undoAvailable(bool)), this, SLOT(updateHasUndo(bool)));
	connect(this, SIGNAL(redoAvailable(bool)), this, SLOT(updateHasRedo(bool)));


	connect(this, SIGNAL(closeEditor()), parent, SLOT(closeCurrentEditor()));
	connect(this, SIGNAL(saveEditor()), parent, SLOT(saveCurrentEditor()));
	connect(this, SIGNAL(toggleComments()), parent, SLOT(editToggleComments()));
	connect(this, SIGNAL(findDialog()), parent->getFindMan(), SLOT(onFindDialog()));
	connect(this, SIGNAL(updateBookmarks(QList<int>)), parent->getBookmarkMan(), SLOT(onUpdateBookmarks(QList<int>)));

	connect(&m_tooltipTimer, SIGNAL(timeout()), this, SLOT(tooltipTimeout()));
	m_tooltipTimer.setSingleShot(true);

	QFont font;
//	font.setFamily("Courier");
	font.setFamily("Source Code Pro");
	font.setFixedPitch(true);
	font.setPointSize(10);
	setFont(font);
	updateLineNumberAreaWidth(0);
	updateHighlights();
	configure(prefs);

	// create the background pixmap
	int size = 10;
	QPainter p(&m_backgroundPixmap);
	int w = m_backgroundPixmap.width()/size;
	int h = m_backgroundPixmap.height()/size;
	for (int x=0; x<w; x++) {
		for (int y=0; y<h; y++) {
			if ((x+y)&1)
				p.fillRect(x*size, y*size, 10, 10, Qt::black);
			else
				p.fillRect(x*size, y*size, 10, 10, Qt::white);
		}
	}
}

void CodeEditor::configure(PreferenceData* prefs) {
	QFont font = this->font();
	QFontMetrics fm(font);
	int tabstop = prefs->getEditorTabWidth();
	QString spaces;
	for (int i=0; i<tabstop; i++)
		spaces += " ";
	this->setTabStopWidth(fm.width(spaces));

	this->setLineWrapMode(prefs->getEditorWrapText() ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap);
	this->m_highlighter = new Highlighter(prefs, this->document());
	this->bAutoIndent = prefs->getAutoIndent();
	this->bAutoBrace = prefs->getAutoBraceCompletion();
	this->bHighlightCurrentLine = prefs->getEditorHighlightCurrentLine();
	this->bHighlightTokens = prefs->getEditorHighlightTokens();
	updateHighlights();
}

void CodeEditor::updateHasUndo(bool)
{
	m_hasUndo = this->isUndoAvailable();
	m_mainWindow->getToolbar()->updateUndoRedo(this);
}

void CodeEditor::updateHasRedo(bool)
{
	m_hasRedo = this->isRedoAvailable();
	m_mainWindow->getToolbar()->updateUndoRedo(this);
}

int CodeEditor::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
	return space;
}

void CodeEditor::updateLineNumberAreaWidth(int newBlockCount)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
	if (newBlockCount != 0 && newBlockCount != m_bookmarkOldBlockCount) {
		int diff = newBlockCount - m_bookmarkOldBlockCount;
		//qDebug() << "updateLineNumberAreaWidth" << newBlockCount << "diff:" << diff;
		int pos = this->textCursor().block().blockNumber();
		bool bookmarksChanged = false;
		for (int i=0; i<m_lineNumberArea->m_bookmarks.size(); i++) {
			if (m_lineNumberArea->m_bookmarks[i] > pos) {
				m_lineNumberArea->m_bookmarks[i] += diff;
				bookmarksChanged = true;
			}
		}
		if (bookmarksChanged)
			emit(updateBookmarks(m_lineNumberArea->m_bookmarks));
		m_lineNumberArea->repaint();
		m_bookmarkOldBlockCount = newBlockCount;
	}

}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
		m_lineNumberArea->scroll(0, dy);
	else
		m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());

	if (rect.contains(viewport()->rect()))
		updateLineNumberAreaWidth(0);

}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent* event) {
	QPainter painter(m_lineNumberArea);
	painter.fillRect(event->rect(), Qt::lightGray);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + (int) blockBoundingRect(block).height();

	while (block.isValid() && top <= event->rect().bottom()) {
		if (block.isVisible() && bottom >= event->rect().top()) {
			if (m_lineNumberArea->hasBookmark(blockNumber+1)) {
				QRect rect(0, top, lineNumberAreaWidth()-1, fontMetrics().height());
				painter.fillRect(rect, Qt::cyan);
			}
			QString number = QString::number(blockNumber + 1);
			painter.setPen(Qt::black);
			painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
							 Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + (int) blockBoundingRect(block).height();
		++blockNumber;
	}
}

void CodeEditor::setBookmarks(QList<int> newBookmarks)
{
	m_lineNumberArea->m_bookmarks = newBookmarks;
	m_lineNumberArea->repaint();
	m_bookmarkOldBlockCount = this->blockCount();
}

void CodeEditor::resizeEvent(QResizeEvent* e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

//void CodeEditor::mousePressEvent(QMouseEvent *mouseEvent) {
//	if (Qt::RightButton == mouseEvent->button()) {
//		QTextCursor textCursor = cursorForPosition(mouseEvent->pos());
//		textCursor.select(QTextCursor::WordUnderCursor);
//		//setTextCursor(textCursor);
//		QString word = textCursor.selectedText();

//		qDebug() << word;
//	}
//	QPlainTextEdit::mousePressEvent(mouseEvent);
//}
void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
	hideTooltip();
	QMenu* menu = createStandardContextMenu(event->pos());
	menu->addSeparator();
	m_contextCursor = cursorForPosition(event->pos());
	m_contextCursor.select(QTextCursor::WordUnderCursor);
	QString t = m_contextCursor.selectedText();
	if (editKeywords.contains(t) && editKeywords[t] == 0)
		menu->addAction(tr("Edit color %1").arg(t), this, SLOT(editColor()));
	else
		menu->addAction(tr("Insert color"), this, SLOT(insertColor()));
	if (editKeywords.contains(t) && editKeywords[t] == 1)
		menu->addAction(tr("Edit color_map"), this, SLOT(editColormap()));
	else
		menu->addAction(tr("Insert color_map"), this, SLOT(insertColormap()));


	menu->exec(this->mapToGlobal(event->pos()));
	delete menu;
}

// We need to capture low level events for the Shift-Tab key and the Ctrl-/ key because
// Qt wants to do something special with them, and I want them.
bool CodeEditor::event(QEvent *e)
{
	if (e->type() == QEvent::ToolTip) {
		QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
		QPoint p = helpEvent->pos();
		p.setX(p.x() - m_lineNumberArea->width());
		QTextCursor cursor = cursorForPosition(p);
		p.setY(p.y() + 10);
		handleHover(helpEvent->globalPos(), p, cursor);
		return true;
	}
	if (e->type() != QEvent::ShortcutOverride)
		return(QPlainTextEdit::event(e));
	QKeyEvent* ke = dynamic_cast<QKeyEvent*>(e);
	if (!ke)
		return(QPlainTextEdit::event(e));
	//qDebug() << "e->type" << ke->key();
	if (ke->key() == Qt::Key_Backtab) {
		qDebug() << "shift-tab";
		e->accept();
		m_mainWindow->editUnindentBlock();
		return(false);
	}
	if (ke->key() == Qt::Key_Slash && (ke->modifiers() & Qt::ControlModifier)) {
		e->accept();
		m_mainWindow->editToggleComments();
		return(false);
	}
	return(QPlainTextEdit::event(e));
}

void CodeEditor::keyPressEvent(QKeyEvent* event)
{
	//qDebug() << "key" << event->key();
	if (event->modifiers() & Qt::ControlModifier) {
		switch (event->key()) {
		case Qt::Key_W:
			emit closeEditor();
			return;
		case Qt::Key_S:
			emit saveEditor();
			return;
		case Qt::Key_F:
			emit findDialog();
			return;
		case Qt::Key_Up:
			handleKeyCtrlUp();
			return;
		case Qt::Key_Down:
			handleKeyCtrlDown();
			return;
		}
	} else {
		switch (event->key()) {
		case Qt::Key_BraceLeft:
			handleKeyBraceLeft(event);
			return;
		case Qt::Key_BraceRight:
			handleKeyBraceRight(event);
			return;
		case Qt::Key_Return:
			handleKeyReturn(event);
			return;
		case Qt::Key_Tab:
			handleKeyTab(event);
			return;
		case Qt::Key_Home:
			if (!(event->modifiers() & Qt::ControlModifier)) {
				handleKeyHome();
				return;
			}
			break;
		}
	}
	QPlainTextEdit::keyPressEvent(event);
}

void CodeEditor::handleKeyCtrlUp()
{
	QScrollBar* sb = verticalScrollBar();
	if (sb->value() > 0)
		sb->setValue(sb->value()-1);
}

void CodeEditor::handleKeyCtrlDown()
{
	QScrollBar* sb = verticalScrollBar();
	if (sb->value() < sb->maximum())
		sb->setValue(sb->value()+1);
}

void CodeEditor::handleKeyBraceLeft(QKeyEvent* event)
{
	QPlainTextEdit::keyPressEvent(event);	// insert the brace
	if (!bAutoBrace)
		return;
	insertPlainText("}");
	this->moveCursor(QTextCursor::PreviousCharacter);
}

void CodeEditor::handleKeyBraceRight(QKeyEvent* event)
{
	if (!bAutoBrace) {
		QPlainTextEdit::keyPressEvent(event);
		return;
	}
	QString t = this->textCursor().block().text();
	int pos = this->textCursor().position();
	int ofs = this->textCursor().block().position();
	if (t[pos-ofs] == '}')
		this->moveCursor(QTextCursor::Right);
	else
		QPlainTextEdit::keyPressEvent(event);	// insert the brace

}
void CodeEditor::handleKeyReturn(QKeyEvent* event)
{
	if (!bAutoIndent) {
		QPlainTextEdit::keyPressEvent(event);
		return;
	}
	QString t = this->textCursor().block().text();
	QString leftMargin;
	for (int i=0; i<t.length(); i++) {
		if (t[i].isSpace())
			continue;
		leftMargin = t.left(i);
		break;
	}
	int pos = this->textCursor().position();
	int ofs = this->textCursor().block().position();
	bool moveBrace = false;
	if (t[pos-ofs] == '}')
		moveBrace = true;
	QString out = "\n";
	out += leftMargin;
	if (moveBrace) {
		out += "\t\n";
		out += leftMargin;
	}
	insertPlainText(out);
	if (moveBrace) {
		this->moveCursor(QTextCursor::Up);
		this->moveCursor(QTextCursor::EndOfBlock);
	}
}
void CodeEditor::handleKeyTab(QKeyEvent* event)
{
	QTextCursor tc = this->textCursor();
	if (!tc.hasSelection()) {
		if (event->modifiers() & Qt::ShiftModifier) {
			this->moveCursor(QTextCursor::Left);
			return;
		}
		if (event->modifiers() & Qt::ControlModifier)
			return;
		QPlainTextEdit::keyPressEvent(event);	// insert the tab
		return;
	}
	handleEditIndent();
}

void CodeEditor::handleKeyHome()
{
	QTextCursor tc = this->textCursor();
	QTextBlock block = tc.block();
	if (!block.length())
		return;
	int i;
	QString s = block.text();
	for (i=0; i<block.length(); i++) {
		if (!s[i].isSpace())
			break;
	}
	int j = tc.position()-block.position();
	tc.movePosition(QTextCursor::StartOfBlock);
	if (j != i)
		tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, i);
	this->setTextCursor(tc);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Manage a two cursor selection for text manipulation
///
class BlockMarker
{
public:
	BlockMarker(QTextCursor t)
		: tc(t), ts(t), te(t) { }

	QString markText() {
		//qDebug() << "tc anchor" << tc.anchor() << "position" << tc.position();
		//te.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, tc.position()-tc.anchor());
		if (tc.anchor() < tc.position()) {
			ts.setPosition(tc.anchor());
			te.setPosition(tc.position());
		} else {
			ts.setPosition(tc.position());
			te.setPosition(tc.anchor());
		}
		startPos = ts.position();
		//qDebug() << "ts anchor" << ts.anchor() << "position" << ts.position();
		//qDebug() << "te anchor" << te.anchor() << "position" << te.position();
		ts.movePosition(QTextCursor::StartOfBlock);
		if (te.atBlockStart()) {
			te.movePosition(QTextCursor::Up);
			te.movePosition((QTextCursor::EndOfBlock));
		}
		//qDebug() << "start" << ts.blockNumber() << ts.block().text();
		//qDebug() << "end  " << te.blockNumber() << te.block().text();
		tp = ts;
		tp.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, te.position()-ts.position());
		return(tp.selectedText());
	}
	void replaceText(const QString& s) {
		tp.insertText(s);
		tp.setPosition(startPos);
		tp.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, s.length());
	}

	QTextCursor tc;
	QTextCursor ts;
	QTextCursor te;
	QTextCursor tp;
	int startPos;
};

void CodeEditor::handleEditIndent()
{
	BlockMarker bm(this->textCursor());
	QString s = bm.markText();
	//qDebug() << "Text:" << s;
	int index = 0;
	do {
		s.insert(index, '\t');
		index = s.indexOf(QChar::ParagraphSeparator, index)+1;
	} while (index != 0);
	bm.replaceText(s);
	this->setTextCursor(bm.tp);
}
void CodeEditor::handleEditUnindent()
{
	BlockMarker bm(this->textCursor());
	QString s = bm.markText();
	//qDebug() << "Text:" << s;
	int index = 0;
	do {
		if (s.at(index) == ' ' || s.at(index) == '\t')
			s.remove(index, 1);
		index = s.indexOf(QChar::ParagraphSeparator, index)+1;
	} while (index != 0);
	bm.replaceText(s);
	this->setTextCursor(bm.tp);
}

void CodeEditor::handleEditToggleComments()
{
	BlockMarker bm(this->textCursor());
	QString s = bm.markText();
	//qDebug() << "Text:" << s;
	int index = 0;
	bool removing = true;
	do {
		if (s.mid(index, 2) != "//") {
			removing = false;
			break;
		}
		index = s.indexOf(QChar::ParagraphSeparator, index)+1;
	} while (index != 0);
	index = 0;
	if (removing) {
		do {
			s.remove(index, 2);
			index = s.indexOf(QChar::ParagraphSeparator, index)+1;
		} while (index != 0);
	} else {
		do {
			s.insert(index, "//");
			index = s.indexOf(QChar::ParagraphSeparator, index)+1;
		} while (index != 0);
	}
	bm.replaceText(s);
	this->setTextCursor(bm.tp);
}

class BraceMatcher
{
public:
	BraceMatcher(CodeEditor* ce)
	{
		valid = false;
		if (ce->textCursor().hasSelection())
			return;
		int pos = ce->textCursor().position();
		QTextBlock curBlock = ce->textCursor().block();
		pos -= curBlock.position();
		BlockData* bd = (BlockData*)curBlock.userData();
		if (!bd)
			return;
		for (int index=0; index < bd->brackets.length(); index++) {
			BracketInfo bi = bd->brackets.at(index);
			if (bi.position == pos) {
				QChar sc = bi.c;
				bool dir = getDir(sc);
				if (dir) {
					QChar ec = getPair(sc);
					int nest = 1;
					++index;
					while (nest != 0) {
						for (; index < bd->brackets.length(); index++) {
							bi = bd->brackets.at(index);
							if (bi.c == sc)
								nest++;
							else if (bi.c == ec)
								nest--;
							if (nest == 0) {
								valid = true;
								position = bi.position+curBlock.position();
								return;
							}
						}
						curBlock = curBlock.next();
						if (!curBlock.isValid())
							return;
						index = 0;
						bd = (BlockData*)curBlock.userData();
					}
				}
			} else if (bi.position == pos-1) {
				QChar ec = bi.c;
				bool dir = getDir(ec);
				if (!dir) {
					QChar sc = getPair(ec);
					int nest = 1;
					--index;
					while (nest != 0) {
						for (; index >= 0; index--) {
							bi = bd->brackets.at(index);
							if (bi.c == ec)
								nest++;
							if (bi.c == sc)
								nest--;
							if (nest == 0) {
								valid = true;
								position = bi.position + curBlock.position();
								return;
							}
						}
						curBlock = curBlock.previous();
						if (!curBlock.isValid())
							return;
						bd = (BlockData*)curBlock.userData();
						index = bd->brackets.length()-1;
					}
				}
			}
		}
	}
	// Given a position and a character, match the block such that
	// "rgb <1,2,3>" with a position of 0 and a char of '<',
	// set the matchedString to "1,2,3" with intermediate comments removed
	// also set the cursor tc to be the selection of the entire text from rgb to trailing > (for text replacement)
	BraceMatcher(CodeEditor* ce, int pos, QChar set)
		: tc(ce->textCursor())
	{
		valid = false;
		matchedString.clear();
		int gInitialPosition = pos;
		tc.setPosition(pos);
		QTextBlock curBlock = tc.block();
		pos -= curBlock.position();
		BlockData* bd = (BlockData*)curBlock.userData();
		if (!bd)
			return;
		int	startPosition;
//		int endPosition;
		for (int index=0; index < bd->brackets.length(); index++) {
			BracketInfo bi = bd->brackets.at(index);
			if (bi.position >= pos) {
				if (bi.c != set)
					return;		// malformed. Looking for < found {
				QChar ec = getPair(set);
				startPosition = bi.position+1;
				QChar sc = bi.c;
				int	nest = 1;
				bool incode = true;
				++index;
				while (nest != 0) {
					for (; index < bd->brackets.length(); index++) {
						bi = bd->brackets.at(index);
						if (bi.c == sc)
							nest++;
						else if (bi.c == ec)
							nest--;
						if (nest == 0) {
							tc.setPosition(startPosition+curBlock.position());
							tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, bi.position-startPosition);
							matchedString += tc.selectedText();
							tc.setPosition(gInitialPosition);
							tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, bi.position+curBlock.position()-gInitialPosition+1);
							return;
						}
					}
					if (incode) {
						tc.setPosition(startPosition);
						tc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, bi.position-startPosition);
						matchedString += tc.selectedText();
					}
					curBlock = curBlock.next();
					bd = (BlockData*)curBlock.userData();
					index = 0;
				}
			}
		}
	}
	bool getDir(QChar c)
	{
		if (c == '{' || c == '[' || c == '(' || c == '<')
			return(true);
		return(false);
	}
	QChar getPair(QChar c)
	{

		if (c == '{') return('}');
		if (c == '}') return('{');
		if (c == '[') return(']');
		if (c == ']') return('[');
		if (c == '(') return(')');
		if (c == ')') return('(');
		if (c == '<') return('>');
		if (c == '>') return('<');
		return(0);
	}
	bool		valid;
	int			position;
	QTextCursor	tc;
	QString		matchedString;

};

void CodeEditor::gotoMatchingBrace()
{
	BraceMatcher bm(this);
	if (bm.valid) {
		QTextCursor tc = this->textCursor();
		int dist = bm.position - tc.position();
		if (dist > 0)
			tc.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor, dist+1);
		else if (dist < 0)
			tc.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, -dist);
		this->setTextCursor(tc);
	}
}

void CodeEditor::gotoLineNumber(int line)
{
	QTextCursor tc = this->textCursor();
	int curLine = tc.blockNumber()+1;
	int dist = line - curLine;
	if (dist > 0)
		tc.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor, dist);
	else if (dist < 0)
		tc.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor, -dist);
	this->setTextCursor(tc);
}

void CodeEditor::updateHighlights()
{
	QList<QTextEdit::ExtraSelection> es;
	highlightCurrentLine(es);
	highlightMatchingTokens(es);
	BraceMatcher bm(this);
	if (bm.valid) {
		QTextEdit::ExtraSelection selection;
		QTextCursor btc = textCursor();
		btc.setPosition(bm.position);
		btc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
		selection.cursor = btc;
		QColor braceColor = QColor(130,238,130);
		selection.format.setBackground(braceColor);
		es.append(selection);

	}

	setExtraSelections(es);
}

void CodeEditor::highlightMatchingTokens(QList<QTextEdit::ExtraSelection>& es)
{
	if (!bHighlightTokens)
		return;
	QString s = this->textCursor().selectedText();
	//qDebug() << "selection:" << s;
	if (!s.isEmpty() &&  s != m_lastHightlightedToken) {
		m_lastHightlightedToken = s;
		QString t = this->toPlainText();		// XXX: woefully inefficient
		int index = 0;
		int	i1;
		while ((i1 = t.indexOf(s, index, Qt::CaseSensitive)) != -1) {
			index = i1+1;
			QTextEdit::ExtraSelection selection;

			QColor lineColor = QColor(238,130,238).lighter(140);
			selection.format.setBackground(lineColor);
			selection.format.setProperty(QTextFormat::FullWidthSelection, true);
			QTextCursor btc = textCursor();
			btc.setPosition(index-1);
			btc.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, s.size());
			selection.cursor = btc;

			es.append(selection);
		}
	}
}

void CodeEditor::highlightCurrentLine(QList<QTextEdit::ExtraSelection>& es)
{
	if (!bHighlightCurrentLine)
		return;
	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		QColor lineColor = QColor(Qt::yellow).lighter(160);
		selection.format.setBackground(lineColor);
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		es.append(selection);
	}
}

void CodeEditor::handleHover(const QPoint& globalPos, QPoint& , QTextCursor& tc)
{
	tc.select(QTextCursor::WordUnderCursor);
	QString t = tc.selectedText();
	if (tc.position() == m_lastTooltipPosition)
		return;			// don't redraw same tooltip
	hideTooltip();
	if (t.isEmpty() || !editKeywords.contains(t))
		return;
	if (t == "color_map") {
		ColormapTooltip* l = new ColormapTooltip(this, Qt::ToolTip);
		BraceMatcher bm(this, tc.position(), '{');
		l->m_colormap.parse(bm.tc.selectedText());
		m_tooltip = l;

	} else {
		ColorTooltip* l = new ColorTooltip(this, Qt::ToolTip);
		BraceMatcher bm(this, tc.position(), '<');
		l->m_color.parse(t, bm.tc.selectedText());
		m_tooltip = l;
	}
	QPoint tooltipOffset(10,10);
	m_tooltip->move(globalPos+tooltipOffset);
	m_tooltip->show();
	m_lastTooltipPosition = tc.position();
	m_tooltipTimer.start(5000);
}

void CodeEditor::editColor()
{
	PovColor color;
	BraceMatcher bm(this, m_contextCursor.anchor(), '<');
	//QString ctype = bm.tc.selectedText();
	//qDebug() << "editColor text" << ctype;
	color.parse(bm.tc.selectedText());
	ColorEditor clre(this, m_mainWindow, color);
	int ret = clre.exec();
	if (ret == QDialog::Accepted) {
		QString s = clre.color().toString();
		qDebug() << "editColor string:" << s;
		bm.tc.insertText(s);
	}
}

void CodeEditor::insertColor()
{
	PovColor color;
	ColorEditor clre(this, m_mainWindow, color);
	int ret = clre.exec();
	if (ret == QDialog::Accepted) {
		QString s = clre.color().toString();
		qDebug() << "insertColor string:" << s;
		textCursor().insertText(s);
	}

}
void CodeEditor::editColormap()
{
	PovColormap colormap;
	BraceMatcher bm(this,  m_contextCursor.anchor(), '{');
	colormap.parse(m_contextCursor.selectedText(), bm.tc.selectedText());
	ColormapEditor clre(this, m_mainWindow, colormap);
	int ret = clre.exec();
	if (ret == QDialog::Accepted) {
		QString s = clre.colormap().toString();
		qDebug() << "editColormap string:" << s;
		QTextCursor tt(bm.tc);
		tt.setPosition(bm.tc.anchor());
		tt.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
		QString indent = m_mainWindow->getInsertMenuMan()->determineLeadingIndent(tt.selectedText());
		s = m_mainWindow->getInsertMenuMan()->applyIndent(s, indent);
		bm.tc.insertText(s);
	}

}
void CodeEditor::insertColormap()
{
	PovColormap colormap;
	ColormapEditor clre(this, m_mainWindow, colormap);
	int ret = clre.exec();
	if (ret == QDialog::Accepted) {
		QString s = clre.colormap().toString();
		qDebug() << "insertColormap string:" << s;
		QTextCursor tt(textCursor());
		tt.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
		QString indent = m_mainWindow->getInsertMenuMan()->determineLeadingIndent(tt.selectedText());
		s = m_mainWindow->getInsertMenuMan()->applyIndent(s, indent);
		textCursor().insertText(s);
	}

}

void CodeEditor::hideTooltip()
{
	if (m_tooltip)
		delete m_tooltip;
	m_tooltip = NULL;
	m_lastTooltipPosition = -1;
}

void CodeEditor::tooltipTimeout()
{
	hideTooltip();
}

///////////////////////////////////////////////////////////////////////////////
void 	ColorTooltip::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.drawPixmap(0,0, m_parent->m_backgroundPixmap);
	QColor c(m_color.toQColor());
	p.fillRect(0,0,100,100, c);

//	painter.fillRect(0, 0, width(), height(), m_color);
}

void  ColormapTooltip::paintEvent(QPaintEvent *)
{
	QPixmap pm(this->width(), this->height());
	m_colormap.paint(&pm, true);
	QPainter p(this);
	p.drawPixmap(0,0, pm);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief LineNumberArea - The left column widget that shows the line numbers
/// \param editor The parent editor we belong to
///
LineNumberArea::LineNumberArea(CodeEditor* editor) : QWidget(editor) {
	m_codeEditor = editor;
}

void LineNumberArea::contextMenuEvent(QContextMenuEvent* event)
{
	qDebug() << "contextMenuEvent" << event->pos();
	QTextBlock block = m_codeEditor->firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int row = event->pos().y() / m_codeEditor->fontMetrics().height();
	int line = row+blockNumber+1;
	qDebug() << blockNumber << row << "line" << line;
	m_contextLine = line;
	QMenu menu(this);
	QString t;
	if (hasBookmark(line))
		t = tr("Remove Bookmark");
	else
		t = tr("Add Bookmark");
	menu.addAction(t, this, SLOT(onBookmarkToggle()), Qt::CTRL + Qt::Key_F2);
	menu.addAction(tr("Next Bookmark"), this, SLOT(onBookmarkNext()), Qt::Key_F2);
	menu.addAction(tr("Previous Bookmark"), this, SLOT(onBookmarkPrevious()), Qt::SHIFT + Qt::Key_F2);
	menu.exec(event->globalPos());
}
void LineNumberArea::onBookmarkToggle()	{ emit m_codeEditor->bookmarkCommand(bmToggle, m_contextLine); }
void LineNumberArea::onBookmarkNext()	{ emit m_codeEditor->bookmarkCommand(bmNext, m_contextLine); }
void LineNumberArea::onBookmarkPrevious(){ emit m_codeEditor->bookmarkCommand(bmPrevious, m_contextLine); }
