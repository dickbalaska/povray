#ifndef _PREFERENCEDATA_H_
#define _PREFERENCEDATA_H_

#include <QString>
#include <QColor>
#include <QKeySequence>
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
	QColor	getColor() { return(color); }
	void	setBold(bool bold) { this->bold = bold; }
	bool	isBold() { return(bold); }
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
		  keyGotoMatchingBrace(Qt::CTRL + Qt::Key_BracketRight)
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

	QList<ShortcutKey>	m_shortcutKeys;
};
inline void GlobalKeys::operator= (const GlobalKeys& other) {
	this->keyOpenPreferences = other.keyOpenPreferences;
	this->keyBookmarkToggle = other.keyBookmarkToggle;
	this->keyBookmarkNext = other.keyBookmarkNext;
	this->keyBookmarkPrevious = other.keyBookmarkPrevious;
	this->keyFindNext = other.keyFindNext;
	this->keyFindPrevious = other.keyFindPrevious;
	this->keyStartRender = other.keyStartRender;
	this->keyErrorNext = other.keyErrorNext;
	this->keyErrorPrevious = other.keyErrorPrevious;
	this->m_shortcutKeys = other.m_shortcutKeys;
}

class PreferenceData {
public:
	PreferenceData() :
		editorWrapText(false),
		editorTabWidth(4),
		editorAutoIndent(true),
		editorAutoBraceCompletion(true),
		editorHighlightCurrentLine(true),
		editorHighlightTokens(true),
		m_useLargeIcons(true)
	{}
	void operator= (const PreferenceData& );

	const QString& getPovrayExecutable() { return(povrayExecutable); }
	void setPovrayExecutable(const QString& exe) { povrayExecutable = exe; }

	const QString& getPovrayIncludes() { return(povrayIncludes); }
	void setPovrayIncludes(const QString& inc) { povrayIncludes = inc; }

	const QString& getPovrayInsertMenu() const { return(povrayInsertMenu); }
	void setPovrayInsertMenu(const QString& ins) { povrayInsertMenu = ins; }

	bool getEditorWrapText() { return(editorWrapText); }
	void setEditorWrapText(bool wrap) { editorWrapText = wrap; }

	int  getEditorTabWidth() const { return(editorTabWidth); }
	void setEditorTabWidth(int w) { editorTabWidth = w; }

	bool getAutoIndent() { return(editorAutoIndent); }
	void setAutoIndent(bool b) { editorAutoIndent = b; }

	bool getAutoBraceCompletion() { return(editorAutoBraceCompletion); }
	void setAutoBraceCompletion(bool b) { editorAutoBraceCompletion = b; }

	bool getEditorHighlightCurrentLine() { return(editorHighlightCurrentLine); }
	void setEditorHighlightCurrentLine(bool b) { editorHighlightCurrentLine = b; }

	bool getEditorHighlightTokens() { return(editorHighlightTokens); }
	void setEditorHighlightTokens(bool b) { editorHighlightTokens = b; }

	EditorColors* getEditorColors() {return(&editorColors); }
	GlobalKeys*		getGlobalKeys() { return(&keys); }

	bool getUseLargeIcons() { return(m_useLargeIcons); }
	void setUseLargeIcons(bool b) { m_useLargeIcons = b; }

private:
	QString	povrayExecutable;
	QString	povrayIncludes;
	QString	povrayInsertMenu;

	bool	editorWrapText;
	int		editorTabWidth;
	bool	editorAutoIndent;
	bool	editorAutoBraceCompletion;
	bool	editorHighlightCurrentLine;
	bool	editorHighlightTokens;
	bool	m_useLargeIcons;
	EditorColors	editorColors;
	GlobalKeys		keys;
};


inline void PreferenceData::operator=(const PreferenceData& other) {
	this->povrayExecutable = other.povrayExecutable;
	this->povrayIncludes = other.povrayIncludes;
	this->povrayInsertMenu = other.povrayInsertMenu;
	this->m_useLargeIcons = other.m_useLargeIcons;
	this->editorWrapText = other.editorWrapText;
	this->editorTabWidth = other.editorTabWidth;
	this->editorAutoIndent = other.editorAutoIndent;
	this->editorAutoBraceCompletion = other.editorAutoBraceCompletion;
	this->editorHighlightCurrentLine = other.editorHighlightCurrentLine;
	this->editorHighlightTokens = other.editorHighlightTokens;
	this->editorColors = other.editorColors;
	this->keys = other.keys;
}

#endif // _PREFERENCEDATA_H_
