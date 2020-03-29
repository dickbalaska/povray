/******************************************************************************
 * codeeditor.h - Extend QPlainTextEdit with POV-Ray customizations
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

#ifndef CODEEDITOR_H_
#define CODEEDITOR_H_

#include "highlighter.h"
#include "povcolor.h"

#include <QPlainTextEdit>
#include <QObject>
#include <QLabel>
#include <QTimer>
#include <QDateTime>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE

class BraceMatcher;
class Highlighter;
class LineNumberArea;
class MainWindow;
class PreferenceData;

struct LineNumberBreakpoint {
	int		mLineNumber;
	bool	mEnabled;
};


class CodeEditor :  public QPlainTextEdit
{
	Q_OBJECT

	friend class BraceMatcher;
	friend class ColorTooltip;
	friend class LineNumberArea;

public:
	CodeEditor(MainWindow *parent, PreferenceData* prefs);
	static void init();

	void	setFilePath(const QString& filePath);
	const QString& getFilePath();
	void	setFileName(const QString& filePath);
	const QString& getFileName();
	void	setFileTime(const QDateTime& fileTime);
	const QDateTime& getFileTime();

	//EditorType	getEditorType();

	void setEditorFocus() { setFocus(); }

	void	lineNumberAreaPaintEvent(QPaintEvent* event);
	int		lineNumberAreaWidth();
	void	configure(const PreferenceData *prefs);

	void	setModified(bool mod);
	bool	isModified();
	bool	isUndoAvailable() { return(m_hasUndo); }
	bool	isRedoAvailable() { return(m_hasRedo); }

	void	setBookmarks(QList<int> newBookmarks);
	void	setBreakpoints(QList<LineNumberBreakpoint> newBreakpoints);

	void	handleEditIndent();
	void	handleEditUnindent();
	void	handleEditToggleComments();


signals:
	void	closeEditor();
	void	saveEditor();
	void	toggleComments();
	void	findDialog();

	void	bookmarkCommand(int bm, int lineNumber=0);
	void	bookmarkToggle(int lineNumber);
	void	bookmarkNext(int lineNumber);
	void	bookmarkPrevious(int lineNumber);
	void	updateBookmarks(const QList<int> marks);
	void	breakpointToggle(int lineNumber);
	void	updateBreakpoints(const QList<LineNumberBreakpoint> marks);

public slots:
	void	gotoMatchingBrace();
	void	gotoLineNumber(int line);

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void keyPressEvent(QKeyEvent* event) override;
	//virtual void mousePressEvent(QMouseEvent *mouseEvent) override;
	virtual	bool event(QEvent *e) override;
	virtual void contextMenuEvent(QContextMenuEvent *event) override;
	virtual void focusInEvent(QFocusEvent* event) override;
	
private slots:
	void	updateLineNumberAreaWidth(int newBlockCount);
	void	updateHighlights();
	void	updateLineNumberArea(const QRect &, int);
	void	hideTooltip();
	void	tooltipTimeout();
	void	editColor();
	void	insertColor();
	void	editColormap();
	void	insertColormap();
	void	updateHasUndo(bool);
	void	updateHasRedo(bool);
	void	handleKeyBraceLeft(QKeyEvent* event);
	void	handleKeyBraceRight(QKeyEvent* event);
	void	handleKeyReturn(QKeyEvent* event);
	void	handleKeyTab(QKeyEvent* event);
	void	handleKeyCtrlUp();
	void	handleKeyCtrlDown();
	void	handleKeyHome();


private:
	void		onFindDialog();
	void		highlightCurrentLine(QList<QTextEdit::ExtraSelection>& es);
	void		highlightDebuggerLine(QList<QTextEdit::ExtraSelection>& es);
	void		highlightMatchingTokens(QList<QTextEdit::ExtraSelection>& es);
	void		handleHover(const QPoint& globalPos, QPoint& pos, QTextCursor &tc);

	MainWindow*		m_mainWindow;
	LineNumberArea*	m_lineNumberArea;
	bool			modified;		// editor has unsaved changes
	Highlighter*	m_highlighter;
	// preferences quick access
	bool			bAutoIndent;
	bool			bAutoBrace;
	bool			bHighlightCurrentLine;
	bool			bHighlightTokens;

	int				m_bookmarkOldBlockCount;	// trigger updating the positions of bookmarks
	QString			m_lastHightlightedToken;
	bool			m_hasUndo;
	bool			m_hasRedo;
	QWidget*		m_tooltip;
	QTimer			m_tooltipTimer;
	int				m_lastTooltipPosition;
	QTextCursor		m_contextCursor;			// where the context menu was popped
	QPixmap			m_backgroundPixmap;
	//QPixmap			m_tooltipPixmap;

	QString			m_filePath;		// absolute path to the file
	QString			m_fileName;		// just the filename displayed in the tab
	QDateTime		m_fileTime;		// 
};

inline void CodeEditor::setModified(bool mod) { modified = mod; }
inline bool CodeEditor::isModified() { return(modified); }
inline void CodeEditor::setFilePath(const QString& filePath) { this->m_filePath = filePath; }
inline const QString& CodeEditor::getFilePath() { return(m_filePath); }
inline void CodeEditor::setFileName(const QString& fileName) { this->m_fileName = fileName; }
inline const QString& CodeEditor::getFileName() { return(m_fileName); }
inline void CodeEditor::setFileTime(const QDateTime& fileTime) { this->m_fileTime = fileTime; }
inline const QDateTime& CodeEditor::getFileTime() { return(m_fileTime); }
//inline EditorType CodeEditor::getEditorType() { return(m_editorType); }


///////////////////////////////////////////////////////////////////////////////
/// \brief LineNumberArea - The left column widget that shows the line numbers
/// \param editor The parent editor we belong to
///
class LineNumberArea : public QWidget
{
	Q_OBJECT

	friend class CodeEditor;
public:
	LineNumberArea(CodeEditor *editor);

	QSize sizeHint() const override {
		return QSize(m_codeEditor->lineNumberAreaWidth(), 0);
	}
	bool hasBookmark(int lineNumber) {
		return(m_bookmarks.contains(lineNumber));
	}
	const LineNumberBreakpoint*	getBreakpoint(int lineNumber);

protected:
	virtual void contextMenuEvent(QContextMenuEvent* event) override;
	void paintEvent(QPaintEvent *event) override {
		m_codeEditor->lineNumberAreaPaintEvent(event);
	}
protected slots:
	void	onBookmarkToggle();
	void	onBookmarkNext();
	void	onBookmarkPrevious();
	void	onBreakpointToggle();

private:
	CodeEditor*					m_codeEditor;
	QList<int>					m_bookmarks;
	QList<LineNumberBreakpoint>	m_breakpoints;
	int							m_contextLine;
};

///////////////////////////////////////////////////////////////////////////////
class ColorTooltip : public QLabel
{
	Q_OBJECT
public:
	ColorTooltip(CodeEditor* parent, Qt::WindowFlags f)
		: QLabel(parent, f),
		  m_parent(parent)
	{ this->resize(80,50);}

protected:
	virtual void 	paintEvent(QPaintEvent *);
public:
	PovColor	m_color;
	CodeEditor*	m_parent;
};

///////////////////////////////////////////////////////////////////////////////
class ColormapTooltip : public QLabel
{
	Q_OBJECT
public:
	ColormapTooltip(CodeEditor* parent, Qt::WindowFlags f)
		: QLabel(parent, f),
		  m_parent(parent)
	{ this->resize(80,50);}

protected:
	virtual void 	paintEvent(QPaintEvent *);
public:
	PovColormap	m_colormap;
	CodeEditor*	m_parent;
};

#endif	// CODEEDITOR_H_
