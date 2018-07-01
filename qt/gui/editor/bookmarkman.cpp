/******************************************************************************
 * bookmarkman.cpp - The Bookmark Manager
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

#include <QDebug>

#include "mainwindow.h"
#include "codeeditor.h"
#include "bookmarkman.h"

BookmarkMan::BookmarkMan(MainWindow* parent)
	: QObject(parent),
	  m_mainWindow(parent)
{
	connect(this, SIGNAL(emitMoveToEditor(QString,int,int)), m_mainWindow, SLOT(moveToEditor(QString,int,int)));

}

BookmarkMan::~BookmarkMan()
{
	foreach (Bookmark* bm, m_bookmarks) {
		delete bm;
	}
}

void	BookmarkMan::onBookmarkCommand(int bm, int lineNumber)
{
	switch (bm) {
	case bmToggle:
		onBookmarkToggle(lineNumber);
		break;
	case bmNext:
		onBookmarkNext(lineNumber);
		break;
	case bmPrevious:
		onBookmarkPrevious(lineNumber);
		break;
	}
}

void	BookmarkMan::onBookmarkToggle(int lineNumber)
{
	qDebug() << "BookmarkMan::onBookmarkToggle" << lineNumber;
	CodeEditor* ce = (CodeEditor*)m_mainWindow->getEditorTabs()->widget(m_mainWindow->getEditorTabs()->currentIndex());
	if (lineNumber == 0)
		lineNumber = ce->textCursor().blockNumber()+1;
	QString path = ce->getFilePath();
	bool found = false;
	Bookmark* bm;
	foreach(bm, m_bookmarks) {
		if (bm->m_pathName == path && bm->m_lineNumber == lineNumber) {
			found = true;
			break;
		}
	}
	if (found) {
		m_bookmarks.removeOne(bm);
		delete bm;
	} else {
		bm = new Bookmark;
		bm->m_pathName = path;
		bm->m_lineNumber = lineNumber;
		addBookmark(bm);
	}
	QList<int> ql = gatherBookmarks(ce);
	ce->setBookmarks(ql);
}

void BookmarkMan::onBookmarkNext(int lineNumber)
{
	qDebug() << "onBookmarkNext" << lineNumber;
	if (!m_mainWindow->getEditorTabs())		// no open editors, no bookmarks
		return;
	CodeEditor* ce = (CodeEditor*)m_mainWindow->getEditorTabs()->widget(m_mainWindow->getEditorTabs()->currentIndex());
	if (lineNumber == 0)
		lineNumber = ce->textCursor().blockNumber()+1;
	Bookmark* bm = NULL;
	foreach(Bookmark* bm2, m_bookmarks) {
		if (bm2->m_pathName == ce->getFilePath()
				&& bm2->m_lineNumber > lineNumber) {
			bm = bm2;
			break;
		}
	}
	if (bm) {
		emit(emitMoveToEditor(bm->m_pathName, bm->m_lineNumber, 0));
		return;
	}
	int wcount = m_mainWindow->getEditorTabs()->count();
	int wstart = m_mainWindow->getEditorTabs()->currentIndex();
	int wcur = wstart +1;
	while (wcur != wstart) {
		if (wcur >= wcount)
			wcur = 0;
		CodeEditor* ce = (CodeEditor*)m_mainWindow->getEditorTabs()->widget(wcur);
		foreach(Bookmark* bm, m_bookmarks) {
			if (bm->m_pathName == ce->getFilePath()) {
				emit(emitMoveToEditor(bm->m_pathName, bm->m_lineNumber, 0));
				return;
			}
		}
		wcur++;
	}
}

void BookmarkMan::onBookmarkPrevious(int lineNumber)
{
	qDebug() << "onBookmarkPrevious" << lineNumber;
	CodeEditor* ce = (CodeEditor*)m_mainWindow->getEditorTabs()->widget(m_mainWindow->getEditorTabs()->currentIndex());
	if (lineNumber == 0)
		lineNumber = ce->textCursor().blockNumber()+1;
	Bookmark* bm = NULL;
	foreach(Bookmark* bm2, m_bookmarks) {
		if (bm2->m_pathName == ce->getFilePath()
				&& bm2->m_lineNumber < lineNumber) {
			bm = bm2;
		}
	}
	if (bm) {
		emit(emitMoveToEditor(bm->m_pathName, bm->m_lineNumber, 0));
		return;
	}
	int wcount = m_mainWindow->getEditorTabs()->count();
	int wstart = m_mainWindow->getEditorTabs()->currentIndex();
	int wcur = wstart -1;
	while (wcur != wstart) {
		if (wcur < 0)
			wcur = wcount-1;
		CodeEditor* ce = (CodeEditor*)m_mainWindow->getEditorTabs()->widget(wcur);
		Bookmark* bm = NULL;
		foreach(Bookmark* bm2, m_bookmarks) {
			if (bm2->m_pathName == ce->getFilePath()) {
				bm = bm2;
			}
		}
		if (bm) {
			emit(emitMoveToEditor(bm->m_pathName, bm->m_lineNumber, 0));
			return;
		}
		wcur--;
	}
}

QList<int>	BookmarkMan::gatherBookmarks(CodeEditor* ce)
{
	QList<int> ql;
	foreach(Bookmark* bm, m_bookmarks) {
		if (ce->getFilePath() == bm->m_pathName)
			ql.append(bm->m_lineNumber);
	}
	return(ql);
}

void BookmarkMan::addBookmark(Bookmark* bm)
{
	QList<Bookmark*>::iterator iter;
	for (iter = m_bookmarks.begin(); iter != m_bookmarks.end(); ++iter) {
		Bookmark* bm2 = *iter;
		if (bm->m_lineNumber > bm2->m_lineNumber) {
			continue;
		}
		m_bookmarks.insert(iter, bm);
		return;
	}
	m_bookmarks.append(bm);
}

void BookmarkMan::onUpdateBookmarks(const QList<int>& list)
{
	CodeEditor* ce = (CodeEditor*)sender();
	QList<Bookmark*>::iterator iter = m_bookmarks.begin();
	while (iter != m_bookmarks.end()) {
		Bookmark* bm = *iter;
		if (bm->m_pathName == ce->getFilePath()) {
			iter = m_bookmarks.erase(iter);
			delete bm;
		} else
			++iter;
	}
	QList<int>::const_iterator liter;
	for (liter = list.begin(); liter != list.end(); ++liter) {
		Bookmark* bm = new Bookmark();
		bm->m_pathName = ce->getFilePath();
		bm->m_lineNumber = *liter;
		addBookmark(bm);
	}
}
