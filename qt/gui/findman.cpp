/******************************************************************************
 * findman.cpp - Manage the Find dialog for qtpovray
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
#include <QtWidgets>

#include "editor/codeeditor.h"
#include "mainwindow.h"
#include "dock/searchman.h"
#include "findman.h"

FindMan::FindMan(MainWindow* parent)
	: QObject(nullptr)
{
	m_mainWindow = parent;
	m_findDialog = nullptr;
}

void FindMan::onFindDialog()
{
	qDebug() << "onFindDialog";
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (!ce)
		return;

	if (!m_findDialog) {
		m_findDialog = new FindDialog(m_mainWindow, this);
	}
	m_findDialog->show();
	//m_findDialog->setFocus();
	m_findDialog->activateWindow();
	m_findDialog->m_findText->setFocus();
	QTextCursor tc = ce->textCursor();
	if (tc.hasSelection())
		m_findDialog->m_findText->setText(tc.selectedText());
	m_findDialog->m_findText->setFocus();
	m_findDialog->m_findText->selectAll();
	m_findDialog->updateButtons();
}

void FindMan::onFindDialogClosed()
{
	qDebug() << "FindMan::onFindDialogClosed";
	m_findDialogPos = m_findDialog->pos();
	m_findDialogSize = m_findDialog->size();
	m_findDialog = nullptr;
}

void FindMan::onFindDialogFind(int findType)
{
	qDebug() << "onFindDialogFind" << findType;
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (!ce)
		return;
	m_lastFindText = m_findDialog->m_findText->text();
	QRegExp* re = nullptr;
	int flags = 0;
	if (m_findDialog->m_caseSensitive->isChecked())
		flags |= QTextDocument::FindCaseSensitively;
	if (m_findDialog->m_wholeWords->isChecked())
		flags |= QTextDocument::FindWholeWords;
	if (!m_findDialog->m_forward->isChecked())
		flags |= QTextDocument::FindBackward;
	m_lastFindRegex = false;
	if (m_findDialog->m_useRegExp->isChecked()) {
		re = new QRegExp(m_lastFindText,
						 flags & QTextDocument::FindCaseSensitively ? Qt::CaseSensitive : Qt::CaseInsensitive);
		m_lastFindRegex = true;
	}
	m_lastFindType = findType;
	m_lastFindFlags = flags;
	switch (findType) {
	case FindType::Replace:
	case FindType::ReplaceFind: {
		QTextCursor tc = ce->textCursor();
		tc.removeSelectedText();
		tc.insertText(m_findDialog->m_replaceText->text());
		if (findType == FindType::ReplaceFind)
			onFindNext();
		break;
	}
		// ReplaceFind falls through to findNext
	case FindType::Find:
		onFindNext();
		break;
	case FindType::FindInFiles:
		//onFindNext();
		m_mainWindow->getSearchMan()->findInFiles(m_lastFindText);
		break;

	}
//	switch (findType) {
//	case FindType::Find:
//		onFindNext();
//		break;
//	case FindType::FindInFiles:
//		m_mainWindow->getSearchMan()->findInFiles(m_lastFindText);
//		break;
//	}
	if (re)
		delete re;
}

void	FindMan::onFindNext()
{
	qDebug() << "FindMan::onFindNext";
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (!ce)
		return;

	switch(m_lastFindType) {
	case FindType::Find:
	case FindType::ReplaceFind:
		if (m_lastFindRegex) {
			QRegExp re(QRegExp(m_lastFindText,
							   m_lastFindFlags & QTextDocument::FindCaseSensitively
							   ? Qt::CaseSensitive : Qt::CaseInsensitive));
			if (!ce->find(re, (QTextDocument::FindFlag)m_lastFindFlags)) {
				int oldPos = ce->textCursor().position();
				QTextCursor tc(ce->document());
				tc.setPosition(0);
				ce->setTextCursor(tc);
				if (!ce->find(re, (QTextDocument::FindFlag)m_lastFindFlags)) {
					tc.setPosition(oldPos);
					ce->setTextCursor(tc);
				}

			}
		} else {
			if (!ce->find(m_lastFindText, (QTextDocument::FindFlag)m_lastFindFlags)) {
				int oldPos = ce->textCursor().position();
				QTextCursor tc(ce->document());
				tc.setPosition(0);
				ce->setTextCursor(tc);
				if (!ce->find(m_lastFindText, (QTextDocument::FindFlag)m_lastFindFlags)) {
					tc.setPosition(oldPos);
					ce->setTextCursor(tc);
				}
			}
		}
		break;
	case FindType::FindInFiles:
		m_mainWindow->getSearchMan()->moveToNext();
		break;
	}
	if (m_findDialog)
		m_findDialog->updateButtons();

}

void	FindMan::onFindPrevious()
{
	qDebug() << "FindMan::onFindPrevious";
	switch(m_lastFindType) {
	case FindType::FindInFiles:
		m_mainWindow->getSearchMan()->moveToPrevious();
		break;
	}
}

void FindMan::moveIsFromSearch()
{
	m_lastFindType = FindType::FindInFiles;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief FindDialog::FindDialog
/// \param parent
///
FindDialog::FindDialog(MainWindow* mainWindow, FindMan* findMan)
	: QDialog(mainWindow),
	  m_mainWindow(mainWindow)
{
	CodeEditor* ce = mainWindow->getCodeEditor();
	m_findMan = findMan;
	this->setWindowTitle(tr("Find"));
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	QGridLayout* glayout = new QGridLayout();
	m_findText = new QLineEdit(this);
	connect(m_findText, SIGNAL(textChanged(QString)), this, SLOT(updateButtons()));
	QLabel* ql = new QLabel(tr("Fi&nd:"));
	ql->setBuddy(m_findText);
	glayout->addWidget(ql, 0, 0);
	glayout->addWidget(m_findText, 0, 1);

	m_replaceText = new QLineEdit(this);
	connect(m_replaceText, SIGNAL(textChanged(QString)), this, SLOT(updateButtons()));
	ql = new QLabel(tr("Re&place with:"));
	ql->setBuddy(m_replaceText);
	glayout->addWidget(ql, 1, 0);
	glayout->addWidget(m_replaceText, 1, 1);
	mainLayout->addLayout(glayout);
	QHBoxLayout* optsLayout = new QHBoxLayout;

	QVBoxLayout* outerRadioLayout = new QVBoxLayout;
	QGroupBox* gridGroupBox = new QGroupBox();
	QVBoxLayout* vbox = new QVBoxLayout;
	m_forward = new QRadioButton(tr("Forw&ard"), gridGroupBox);
	m_forward->setChecked(true);
	vbox->addWidget(m_forward);
	QRadioButton* rb = new QRadioButton(tr("Re&verse"), gridGroupBox);
	vbox->addWidget(rb);
	//vbox->addStretch(1);
	gridGroupBox->setLayout(vbox);
	outerRadioLayout->addWidget(gridGroupBox);
	outerRadioLayout->addStretch(1);
	optsLayout->addLayout(outerRadioLayout);

	QVBoxLayout* optsChecksLayout = new QVBoxLayout();
	m_useRegExp = new QCheckBox(tr("Use RegE&xp"), this);
	optsChecksLayout->addWidget(m_useRegExp);
	m_caseSensitive = new QCheckBox(tr("&Case Sensitive"), this);
	optsChecksLayout->addWidget(m_caseSensitive);
	m_wholeWords = new QCheckBox(tr("Match &whole words"), this);
	optsChecksLayout->addWidget(m_wholeWords);
	optsChecksLayout->addStretch(1);
	optsLayout->addLayout(optsChecksLayout);
	mainLayout->addLayout(optsLayout);

	glayout = new QGridLayout;
	QSignalMapper* signalMapper = new QSignalMapper(this);
	m_findButton = new QPushButton("&Find", this);
	glayout->addWidget(m_findButton, 0, 0);
	connect(m_findButton, SIGNAL(pressed()), signalMapper, SLOT(map()));
	signalMapper->setMapping(m_findButton, FindType::Find);
	m_findInFilesButton = new QPushButton("&Search in Files", this);
	glayout->addWidget(m_findInFilesButton, 1, 0);
	connect(m_findInFilesButton, SIGNAL(pressed()), signalMapper, SLOT(map()));
	signalMapper->setMapping(m_findInFilesButton, FindType::FindInFiles);
	m_replaceFindButton = new QPushButton("Replace/Fin&d", this);
	glayout->addWidget(m_replaceFindButton, 1, 1);
	connect(m_replaceFindButton, SIGNAL(pressed()), signalMapper, SLOT(map()));
	signalMapper->setMapping(m_replaceFindButton, FindType::ReplaceFind);
	m_replaceButton = new QPushButton("&Replace", this);
	glayout->addWidget(m_replaceButton, 0, 1);
	connect(m_replaceButton, SIGNAL(pressed()), signalMapper, SLOT(map()));
	signalMapper->setMapping(m_replaceButton, FindType::Replace);
	m_replaceAllButton = new QPushButton("ReplaceA&ll", this);
	glayout->addWidget(m_replaceAllButton, 2, 1);
	connect(m_replaceAllButton, SIGNAL(pressed()), signalMapper, SLOT(map()));
	signalMapper->setMapping(m_replaceAllButton, FindType::ReplaceAll);
	mainLayout->addLayout(glayout);

	connect(signalMapper, SIGNAL(mapped(int)), m_findMan, SLOT(onFindDialogFind(int)));
	connect(this, SIGNAL(closeSignal()), m_findMan, SLOT(onFindDialogClosed()));
	connect(this, SIGNAL(rejected()), m_findMan, SLOT(onFindDialogClosed()));
	connect(this, SIGNAL(accepted()), m_findMan, SLOT(onFindDialogClosed()));
	//connect(this, SIGNAL(done()), m_findMan, SLOT(onFindDialogClosed()));
	if (!m_findMan->m_findDialogPos.isNull()) {
		this->move(m_findMan->m_findDialogPos);
		this->resize(m_findMan->m_findDialogSize);
	}
	QTextCursor tc = ce->textCursor();
	if (tc.hasSelection()) {
		m_findText->setText(tc.selectedText());
	}
	updateButtons();
}

void FindDialog::updateButtons()
{
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (!ce) {
		m_findButton->setEnabled(false);
		m_findInFilesButton->setEnabled(false);
		m_replaceButton->setEnabled(false);
		m_replaceFindButton->setEnabled(false);
		m_replaceAllButton->setEnabled(false);
		return;
	}
	bool bFind = false;
	if (!m_findText->text().isEmpty())
		bFind = true;
	m_findButton->setEnabled(bFind);
	m_findInFilesButton->setEnabled(bFind);

	QTextCursor tc = ce->textCursor();
	bool bReplace = false;
	if (tc.hasSelection() && !m_replaceText->text().isEmpty())
		bReplace = true;
	m_replaceButton->setEnabled(bReplace);
	m_replaceFindButton->setEnabled(bReplace);
	m_replaceAllButton->setEnabled(bReplace);

}
void FindDialog::closeEvent(QCloseEvent* event)
{
	qDebug() << "FindDialog::closeEvent";
//	emit closeSignal();
	QDialog::closeEvent(event);
}
