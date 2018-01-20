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
