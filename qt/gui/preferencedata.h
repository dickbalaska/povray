/******************************************************************************
 * preferencedata.h - Define the user's and system's preferences
 *
 * qtpovray - A Qt GUI IDE frontend for POV-Ray
 * Copyright(c) 2017 - Dick Balaska, and BuckoSoft.
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

#ifndef PREFERENCEDATA_H_
#define PREFERENCEDATA_H_

#include <QString>
#include <QColor>
#include <QKeySequence>
#include <QFont>
#include <QDebug>

class Highlight {
public:
	Highlight() {color = Qt::black; bold = false;}
	Highlight(QColor color, bool bold) {
		this->color = color; this->bold = bold; }
	bool	operator==(const Highlight* other) {
		return(this->color == other->color && this->bold == other->bold);
	}
	void	operator=(const Highlight& other) {
		this->color = other.color; this->bold = other.bold;
	}
	void	setColor(QColor color) { this->color = color; }
	const QColor	getColor() const { return(color); }
	void	setBold(bool bold) { this->bold = bold; }
	bool	isBold() const { return(bold); }
private:
	QColor color;
	bool	bold;
};

class EditorColors {
public:
	EditorColors()
		: common(QColor(Qt::darkMagenta), false),		// note: these are overwritten from preferences
		  math(QColor(Qt::darkRed), false),
		  modifier(QColor(Qt::darkMagenta), false),
		  object(QColor(Qt::darkYellow), true),
		  texture(QColor(Qt::darkYellow), false),
		  color(QColor(Qt::red), false),
		  comment(QColor(Qt::darkGreen), false),
		  string(QColor(Qt::darkBlue), false)
	{}
	void operator= (const EditorColors& );

	Highlight	common;
	Highlight	math;
	Highlight	modifier;
	Highlight	object;
	Highlight	texture;
	Highlight	color;
	Highlight	comment;
	Highlight	string;
};
inline void EditorColors::operator= (const EditorColors& other) {
	this->common = other.common;
	this->math = other.math;
	this->modifier = other.modifier;
	this->object = other.object;
	this->texture = other.texture;
	this->color = other.color;
	this->comment = other.comment;
	this->string = other.string;
}
#define PrefType(_index, _key, _label) \
	enum { pt_ ## _key = _index }; \
	static QString s_##_key(#_label);

//static QString s_StartRender	("StartRender");

PrefType(0, StartRender, "Start/Stop Render")

class ShortcutKey : QObject {
	Q_OBJECT
public:
	ShortcutKey();
	ShortcutKey(const QString& type, const QKeySequence& key) : QObject(),
		m_type(type),
		m_keySequence(key)
	{}
	ShortcutKey(const ShortcutKey& other) : QObject() {
		this->m_type = other.m_type; this->m_keySequence = other.m_keySequence; }
	virtual ~ShortcutKey() {}
	ShortcutKey& operator=(const ShortcutKey& other) {
		m_type = other.m_type;
		m_keySequence = other.m_keySequence;
		return(*this);
	}

	QString			m_type;
	QKeySequence	m_keySequence;
};

class GlobalKeys {
public:
	GlobalKeys()
		: keyOpenPreferences(Qt::ALT + Qt::Key_C),
		  keyBookmarkToggle(Qt::CTRL + Qt::Key_F2),
		  keyBookmarkNext(Qt::Key_F2),
		  keyBookmarkPrevious(Qt::SHIFT + Qt::Key_F2),
		  keyBreakpointToggle(Qt::Key_F9),
		  keyFindNext(Qt::Key_F3),
		  keyFindPrevious(Qt::SHIFT + Qt::Key_F3),
		  keyStartRender(Qt::CTRL + Qt::ALT + Qt::Key_R),
		  keyErrorNext(Qt::Key_F4),
		  keyErrorPrevious(Qt::SHIFT + Qt::Key_F4),
		  keySaveAll(Qt::CTRL + Qt::SHIFT + Qt::Key_S),
		  keyIndentBlock(Qt::Key_Tab),
		  keyUnindentBlock(Qt::SHIFT + Qt::Key_Tab),
		  keyToggleComment(Qt::CTRL + Qt::Key_Slash),
		  keyGotoLineNumber(Qt::CTRL + Qt::Key_L),
		  keyGotoMatchingBrace(Qt::CTRL + Qt::Key_BracketRight),
		  keyDebuggerStep(Qt::Key_F10)
	{
		m_shortcutKeys
				<< ShortcutKey(s_StartRender, QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_R))
				<< ShortcutKey(s_StartRender, QKeySequence(Qt::CTRL + Qt::Key_G))
				   ;
	}
	void operator= (const GlobalKeys& );

	QKeySequence	keyOpenPreferences;
	QKeySequence	keyBookmarkToggle;
	QKeySequence	keyBookmarkNext;
	QKeySequence	keyBookmarkPrevious;
	QKeySequence	keyBreakpointToggle;
	QKeySequence	keyFindNext;
	QKeySequence	keyFindPrevious;
	QKeySequence	keyStartRender;
	QKeySequence	keyErrorNext;
	QKeySequence	keyErrorPrevious;
	QKeySequence	keySaveAll;
	QKeySequence	keyIndentBlock;
	QKeySequence	keyUnindentBlock;
	QKeySequence	keyToggleComment;
	QKeySequence	keyGotoLineNumber;
	QKeySequence	keyGotoMatchingBrace;
	QKeySequence	keyDebuggerStep;

	QList<ShortcutKey>	m_shortcutKeys;
};

inline void GlobalKeys::operator= (const GlobalKeys& other) {
	this->keyOpenPreferences = other.keyOpenPreferences;
	this->keyBookmarkToggle = other.keyBookmarkToggle;
	this->keyBookmarkNext = other.keyBookmarkNext;
	this->keyBookmarkPrevious = other.keyBookmarkPrevious;
	this->keyBreakpointToggle = other.keyBreakpointToggle;
	this->keyFindNext = other.keyFindNext;
	this->keyFindPrevious = other.keyFindPrevious;
	this->keyStartRender = other.keyStartRender;
	this->keyErrorNext = other.keyErrorNext;
	this->keyErrorPrevious = other.keyErrorPrevious;
	this->keyDebuggerStep = other.keyDebuggerStep;
	this->m_shortcutKeys = other.m_shortcutKeys;
}

class PreferenceData {
	friend class MainWindow;

public:
	PreferenceData() :
		editorWrapText(false),
		editorTabWidth(4),
		editorAutoIndent(true),
		editorAutoBraceCompletion(true),
		editorHighlightCurrentLine(true),
		editorHighlightTokens(true),
		m_useLargeIcons(true),
		m_useEditorViMode(true),
	    m_alwaysReloadFiles(true)
	{
		m_editorFont.setFamily("Source Code Pro");
		m_editorFont.setPointSize(10);
		m_editorFont.setFixedPitch(true);

	}
	void operator= (const PreferenceData& );

	const QString& getPovrayIncludes() const { return(povrayIncludes); }
	void setPovrayIncludes(const QString& inc) { povrayIncludes = inc; }

	const QString& getPovrayInsertMenu() const { return(m_povrayInsertMenu); }
	void setPovrayInsertMenu(const QString& ins) { m_povrayInsertMenu = ins; }

	const QString& getUserInsertMenu() const { return(m_userInsertMenu); }
	void setUserInsertMenu(const QString& ins) { m_userInsertMenu = ins; }

	const QString& getPovraySceneDirectory() const { return(povraySceneDirectory); }
	void setPovraySceneDirectory(const QString& ins) { povraySceneDirectory = ins; }

	const QString& getPovrayHelpDirectory() const { return(povrayHelpDirectory); }
	void setPovrayHelpDirectory(const QString& inh) { povrayHelpDirectory = inh; }

	const QString& getQtpovrayHelpDirectory() const { return(qtpovrayHelpDirectory); }
	void setQtpovrayHelpDirectory(const QString& inh) { qtpovrayHelpDirectory = inh; }

	bool getEditorWrapText() const { return(editorWrapText); }
	void setEditorWrapText(bool wrap) { editorWrapText = wrap; }

	int  getEditorTabWidth() const { return(editorTabWidth); }
	void setEditorTabWidth(int w) { editorTabWidth = w; }

	bool getAutoIndent() const { return(editorAutoIndent); }
	void setAutoIndent(bool b) { editorAutoIndent = b; }

	bool getAutoBraceCompletion() const { return(editorAutoBraceCompletion); }
	void setAutoBraceCompletion(bool b) { editorAutoBraceCompletion = b; }

	bool getEditorHighlightCurrentLine() const { return(editorHighlightCurrentLine); }
	void setEditorHighlightCurrentLine(bool b) { editorHighlightCurrentLine = b; }

	bool getEditorHighlightTokens() const { return(editorHighlightTokens); }
	void setEditorHighlightTokens(bool b) { editorHighlightTokens = b; }

	const EditorColors* getEditorColors() const {return(&editorColors); }
	EditorColors* getEditorColorsNC() {return(&editorColors); }
	GlobalKeys*		getGlobalKeys() { return(&keys); }

	bool getUseLargeIcons() const { return(m_useLargeIcons); }
	void setUseLargeIcons(bool b) { m_useLargeIcons = b; }

	bool getAlwaysReloadFiles() const { return(m_alwaysReloadFiles); }
	void setAlwaysReloadFiles(bool b) { m_alwaysReloadFiles = b; }

	bool getUseEditorViMode() const { return(m_useEditorViMode); }
	void setUseEditorViMode(bool b) { m_useEditorViMode = b; }

	QString getEditorFontFamily() const { return(m_editorFont.family()); }
	void setEditorFontFamily(const QString& font) { m_editorFont.setFamily(font); }
	void setEditorFontPointSize(int size) { m_editorFont.setPointSize(size); }

	const QFont getEditorFont() const { return(m_editorFont); }
	void setEditorFont(const QFont& font) { m_editorFont = font; }

	const QFont getConsoleFont() const { return(m_consoleFont); }
	void setConsoleFont(const QFont& font) { m_consoleFont = font; }

private:
	QString	povrayIncludes;
	QString	m_povrayInsertMenu;
	QString	m_userInsertMenu;
	QString	povraySceneDirectory;
	QString	povrayHelpDirectory;
	QString	qtpovrayHelpDirectory;

	bool	editorWrapText;
	int		editorTabWidth;
	bool	editorAutoIndent;
	bool	editorAutoBraceCompletion;
	bool	editorHighlightCurrentLine;
	bool	editorHighlightTokens;
	bool	m_useLargeIcons;
	bool	m_useEditorViMode;
	bool	m_alwaysReloadFiles;
//	QString	m_editorFontFamily;
//	int		m_editorFontPointSize;
	QFont	m_editorFont;
	QFont	m_consoleFont;

	EditorColors	editorColors;
	GlobalKeys		keys;
};


inline void PreferenceData::operator=(const PreferenceData& other) {
	this->povrayIncludes = other.povrayIncludes;
	this->m_povrayInsertMenu = other.m_povrayInsertMenu;
	this->m_userInsertMenu = other.m_userInsertMenu;
	this->povraySceneDirectory = other.povraySceneDirectory;
	this->povrayHelpDirectory = other.povrayHelpDirectory;
	this->qtpovrayHelpDirectory = other.qtpovrayHelpDirectory;
	this->m_useLargeIcons = other.m_useLargeIcons;
	this->m_alwaysReloadFiles = other.m_alwaysReloadFiles;
	this->editorWrapText = other.editorWrapText;
	this->editorTabWidth = other.editorTabWidth;
	this->editorAutoIndent = other.editorAutoIndent;
	this->editorAutoBraceCompletion = other.editorAutoBraceCompletion;
	this->editorHighlightCurrentLine = other.editorHighlightCurrentLine;
	this->editorHighlightTokens = other.editorHighlightTokens;
	this->m_editorFont = other.m_editorFont;
	this->m_consoleFont = other.m_consoleFont;
	this->editorColors = other.editorColors;
	this->keys = other.keys;
}

#endif // PREFERENCEDATA_H_
