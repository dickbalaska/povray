/******************************************************************************
 * preferences.h - The dialog box to control user preferences
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

#ifndef _PREFERENCES_H_
#define _PREFERENCES_H_

#include <QDebug>
#include <QDialog>
#include <QVBoxLayout>
#include <QIcon>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include "preferencedata.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTextEdit;
class QTabWidget;
class QLabel;
class QCheckBox;
class QSpinBox;
class QPushButton;
class QGroupBox;
class QKeySequenceEdit;
QT_END_NAMESPACE

class KeyLabel;
class KeyLineEdit;
class MainWindow;
class Preferences;

///////////////////////////////////////////////////////////////////////////////////////
class PovrayTab : public QWidget {
	Q_OBJECT
public:
	PovrayTab(Preferences *parent);
	void validateData();
	QTextEdit* getPovrayBanner() { return(povrayBanner); }

public slots:
#ifdef USE_WEBSOCKETS
	void browseExeClicked(bool b);
	void textExeEdited(const QString& text);
#endif
	void browseIncClicked(bool b);
	void textIncEdited(const QString& text);
	void browseInsClicked(bool b);
	void textInsEdited(const QString& text);
	void benchmarkButtonClicked(bool);

private:
#ifdef USE_WEBSOCKETS
	void validateExe();
#endif
	void validateInc();
	void validateIns();

	Preferences* parent;

#ifdef USE_WEBSOCKETS
	QLineEdit*	povrayExecutable;
	QLabel*		povrayExecutableStatus;
#endif
	QLineEdit*	povrayIncludes;
	QLabel*		povrayIncludesStatus;
	QLineEdit*	povrayInsertMenu;
	QLabel*		povrayInsertMenuStatus;
	QTextEdit*	povrayBanner;		// output of talking to POV-Ray
};

///////////////////////////////////////////////////////////////////////////////////////
class EditorTab : public QWidget {
	Q_OBJECT
public:
	EditorTab(Preferences* parent);

public slots:

	void	largeIconButtonChanged(int state);
	void	wrapButtonChanged(int state);
	void	tabWidthChanged(int width);
	void	autoIndentChanged(int state);
	void	autoBraceChanged(int state);
	void	highlightLineChanged(int state);
	void	highlightTokensChanged(int state);

private:
	QCheckBox*	largeIconButton;
	QCheckBox*	wrapButton;
	QSpinBox*	tabWidthSpinner;
	QCheckBox*	autoIndentButton;
	QCheckBox*	autoBraceButton;
	QCheckBox*	highlightLineButton;
	QCheckBox*	highlightTokensButton;

	Preferences* parent;
};

///////////////////////////////////////////////////////////////////////////////////////
class ColorButton : public QPushButton {
	Q_OBJECT
public:
	ColorButton(const QIcon& icon, QWidget* parent, Highlight* highlight);

public slots:
	void buttonClicked(bool);
	void boldChecked(int);

private:
	Highlight*	highlight;
};


///////////////////////////////////////////////////////////////////////////////////////
class ColorTab : public QWidget {
	Q_OBJECT
public:
	ColorTab(Preferences* parent);
	void addButton(QGridLayout *layout, ColorButton* button, int row,
				   Highlight *highlight, const QString& label);

private:
	ColorButton*	defaultButton;
	ColorButton*	commentButton;
	ColorButton*	stringButton;
	ColorButton*	commonButton;
	ColorButton*	objectButton;
	ColorButton*	modifierButton;
	ColorButton*	textureButton;
	ColorButton*	colorButton;
	ColorButton*	mathButton;

	Preferences* parent;
};

///////////////////////////////////////////////////////////////////////////////////////
class KeyTab : public QWidget {
	Q_OBJECT

	friend class KeyLabel;
	friend class KeyLineEdit;

public:
	KeyTab(Preferences* parent);

public slots:
	void	newKeyCancel();
	void	newKeyOk();

private:
	void addKey(QGridLayout* layout, int row, const QString& text, QKeySequence *key);

	Preferences*		m_parent;
	QGroupBox*			m_newKeyGroupBox;
	QKeySequenceEdit*	m_newKeyEdit;
	KeyLabel*			m_activeKeyLabel;
};

///////////////////////////////////////////////////////////////////
class KeyLabelLabel : public QLabel {
	Q_OBJECT
public:
	explicit KeyLabelLabel(const QString& text, KeyTab* parent)
		: QLabel(text, parent){}
	virtual ~KeyLabelLabel() {}
protected:
	virtual void mousePressEvent(QMouseEvent *event);
};

///////////////////////////////////////////////////////////////////
class KeyLabel : public QLabel {
	Q_OBJECT

	friend class KeyTab;
	friend class KeyLabelLabel;

public:
	explicit KeyLabel(const QString& text, KeyTab* parent)
		: QLabel(text, parent) {m_parent = parent;}
	virtual ~KeyLabel() {}
protected:
	virtual void mousePressEvent(QMouseEvent *);
private:
	QKeySequence*	m_data;
	KeyTab*			m_parent;
};

///////////////////////////////////////////////////////////////////
//class KeyLineEdit : public QLineEdit {
//	Q_OBJECT
//public:
//	explicit KeyLineEdit(KeyTab* parent)
//		: QLineEdit(parent) {m_parent = parent;}

//protected:
//	virtual void keyPressEvent(QKeyEvent* event);

//private:
//	int*	m_data;
//	KeyTab* m_parent;

//};
///////////////////////////////////////////////////////////////////////////////////////
class Preferences : public QDialog
{
	Q_OBJECT
	friend class PovrayTab;
	friend class EditorTab;
	friend class ColorTab;
	friend class KeyTab;

public:
	Preferences(MainWindow* parent, PreferenceData* data);

	static bool validateInc(const QString& file);
	static bool validateIns(const QString& file);
private:
	//MainWindow*	mainWindow;
	QIcon* iconOk;
	QIcon* iconBad;

	PreferenceData*	prefData;
	MainWindow*	mainWindow;
	QTabWidget* tabWidget;
	PovrayTab*	povrayTab;
	EditorTab*	editorTab;
	ColorTab*	colorTab;
	KeyTab*		keyTab;
};

#endif // _PREFERENCES_H_
