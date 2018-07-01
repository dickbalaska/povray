/******************************************************************************
 * findman.h - Manage the Find dialog for qtpovray
 *
 * qtpovray - A Qt IDE frontend for POV-Ray
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
#ifndef FINDMAN_H
#define FINDMAN_H

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QDebug>

class MainWindow;
class FindMan;
class SearchMan;
class Workspace;

///////////////////////////////////////////////////////////////////////////////
/// \brief The FindDialog class
///
class FindDialog : public QDialog
{
	Q_OBJECT

	friend class FindMan;
	friend class SearchMan;

public:
	FindDialog(MainWindow* mainWindow, FindMan* findMan);

signals:
	void	closeSignal();

public slots:
	void updateButtons();

protected:
	virtual void closeEvent(QCloseEvent* event);

private:
	MainWindow*		m_mainWindow;
	FindMan*		m_findMan;
	QLineEdit*		m_findText;
	QLineEdit*		m_replaceText;
	QCheckBox*		m_useRegExp;
	QCheckBox*		m_caseSensitive;
	QCheckBox*		m_wholeWords;
	QRadioButton*	m_forward;
	QPushButton*	m_findButton;
	QPushButton*	m_findInFilesButton;
	QPushButton*	m_replaceButton;
	QPushButton*	m_replaceFindButton;
	QPushButton*	m_replaceAllButton;
};

///////////////////////////////////////////////////////////////////////////////
/// \brief The FindMan class
///
class FindMan : public QObject
{
	Q_OBJECT

	friend class FindDialog;
	friend class Workspace;

public:
	explicit FindMan(MainWindow* parent);

	const FindDialog* getFindDialog() const { return(m_findDialog); }

signals:

public slots:
	void	onFindDialog();
	void	onFindDialogClosed();
	void	onFindDialogFind(int findType);
	void	onFindNext();
	void	onFindPrevious();
	void	moveIsFromSearch();

private:
	MainWindow*		m_mainWindow;
	FindDialog*		m_findDialog;
	QPoint			m_findDialogPos;
	QSize			m_findDialogSize;
	QString			m_lastFindText;
	int				m_lastFindType;
	int				m_lastFindFlags;
	bool			m_lastFindRegex;
};


#endif // FINDMAN_H
