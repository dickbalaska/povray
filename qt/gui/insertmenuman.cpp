/******************************************************************************
 * insertmenuman.cpp - Insert Menu Manager
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
#include <QtWidgets>

#include "dock/maintoolbar.h"
#include "editor/codeeditor.h"
#include "mainwindow.h"
#include "insertmenuman.h"

#define	_INSDEBUG	false
InsertMenuMan::InsertMenuMan(MainWindow* parent)
	: QObject(parent),
	  m_mainWindow(parent)
{
	m_preview = NULL;
	QMenu* rootMenu = m_mainWindow->getToolbar()->getInsertMenu();
	connect(rootMenu, SIGNAL(aboutToHide()), SLOT(menuHidden()));
}

void InsertMenuMan::populateMenu(bool enable)
{
	QMenu* rootMenu = m_mainWindow->getToolbar()->getInsertMenu();
	rootMenu->clear();
	if (!enable)
		return;
	QDir rootDir(m_mainWindow->getPreferenceData().getPovrayInsertMenu());
	QFileInfoList rootFiles = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	foreach (QFileInfo fi, rootFiles) {
		if (_INSDEBUG) qDebug() << "File:" << fi.fileName();
		QString s = fixFileName(fi.fileName());
		QMenu* submenu = new QMenu(s, m_mainWindow);
		populateSubMenu(submenu, fi);
		rootMenu->addMenu(submenu);
	}
}

QString InsertMenuMan::fixFileName(const QString& name)
{
	QString s;
	if (name.mid(2, 3) == " - ")
		s = name.mid(5);
	else
		s = name;
	return(s);
}

void InsertMenuMan::populateSubMenu(QMenu* menu, QFileInfo fi)
{
	QDir dir(fi.absoluteFilePath());
	QFileInfoList files = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
	foreach (QFileInfo fi, files) {
		QString s = fixFileName(fi.fileName());
		if (fi.isDir()) {
			QMenu* submenu = new QMenu(s, m_mainWindow);
			populateSubMenu(submenu, fi);
			menu->addMenu(submenu);
			continue;
		}
		if (s.endsWith(".txt")) {
			if (_INSDEBUG) qDebug() << "File:" << fi.fileName();
			if (s == "-.txt") {
				menu->addSeparator();
				continue;
			}
			InsertMenuAction* action = new InsertMenuAction(s.left(s.length()-4), m_mainWindow);
			connect(action, SIGNAL(triggered(bool)), this, SLOT(itemSelected()));
			connect(action, SIGNAL(hovered()), this, SLOT(itemHovered()));
			action->m_filePath = fi.absoluteFilePath();
			menu->addAction(action);
		}
	}
}

void InsertMenuMan::itemSelected()
{
	InsertMenuAction* action =  qobject_cast<InsertMenuAction*>(sender());
	if (!action)
		return;
	QFile file(action->m_filePath);
	if (!file.open(QFile::ReadOnly | QFile::Text))
		return;
	QTextStream in(&file);
	CodeEditor* ce = m_mainWindow->getCodeEditor();
	if (!ce)
		return;
	QString s = in.readAll();
	s = processSubstitutions(s);
	s = normalizeIndenting(s);
//	QTextCursor tc = ce->textCursor();
//	QTextBlock b = tc.block();
//	int pos = tc.position()-b.position();
	QTextCursor tt(ce->textCursor());
	tt.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
	QString leadingIndent = determineLeadingIndent(tt.selectedText());
	s = applyIndent(s, leadingIndent);
	ce->textCursor().insertText(s);
}

QString InsertMenuMan::normalizeIndenting(const QString &in)
{
	int index = 0;
	int min = -1;
	while (index < in.length()) {
		int count = 0;
		for (; index < in.length(); index++) {
			QChar c = in[index];
			if (c == '\n')
				break;
			if (c.isSpace())
				count++;
			else
				break;
		}
		if (min == -1 || count < min)
			min = count;
		for (; index < in.length(); index++) {
			if (in[index] == '\n') {
				index++;
				break;
			}
		}
	}
	index = 0;
	QString out = in;
	while (index < out.length()) {
		out = out.remove(index, min);
		index++;
		while (index < out.length()) {
			if (out[index++] == '\n')
				break;
		}
	}
	return(out);
}

QString	InsertMenuMan::determineLeadingIndent(const QString& leader)
{
	int tabstop = m_mainWindow->getPreferenceData().getEditorTabWidth();
	int col = 0;
	for (int i=0; i<leader.length(); i++) {
		QChar c = leader[i];
		if (c == '\t') {
			int j = col/tabstop;
			col = (j+1)*tabstop;
		} else
			col++;
	}
	int i = col / tabstop;
	int r = col % tabstop;
	QString out;
	for (int j=0; j<i; j++)
		out.append('\t');
	for (int j=0; j<r; j++)
		out.append(' ');
	qDebug() << "leader in:" << leader;
	qDebug() << "leader out:" << out;
	return(out);
}

QString	InsertMenuMan::applyIndent(const QString& in, const QString& indent)
{
	QString out = in;
	for (int i=0; i<out.length();) {
		if (out[i++] == '\n' && i < out.length())
			out.insert(i, indent);
	}
	return(out);
}

void InsertMenuMan::menuHidden()
{
	if (m_preview) {
		delete m_preview;
		m_preview = NULL;
	}
}
QPixmap* pm;
void InsertMenuMan::itemHovered()
{
	InsertMenuAction* action =  qobject_cast<InsertMenuAction*>(sender());
	if (!action)
		return;
	QString gfile = action->m_filePath.left(action->m_filePath.length()-4) + ".jpg";
	if (m_preview && m_preview->m_loadedFile == gfile)
		return;
	QMenu* menu = m_mainWindow->getToolbar()->getInsertMenu();
	QAction* qa = menu->activeAction();
	while (qa->menu()) {
		menu = qa->menu();
		qa = menu->activeAction();
	}
	if (!qa) {
		if (_INSDEBUG) qDebug() << "No QAction";
		return;
	}
	pm = new QPixmap(gfile);
	if (!m_preview) {
		//QDesktopWidget* desktop = QApplication::desktop();
		m_preview = new InsertMenuPreview(menu->parentWidget(), Qt::FramelessWindowHint);
		m_preview->setMinimumSize(160,120);
		m_preview->setVisible(true);
		if (_INSDEBUG) qDebug() << "Created window";
	}
	m_preview->m_loadedFile = gfile;
	m_preview->m_label->setPixmap(*pm);
	m_preview->m_label->setMinimumSize(pm->size());
	if (_INSDEBUG) qDebug() << "";
	if (_INSDEBUG) qDebug() << "gfile" << gfile;
	if (_INSDEBUG) qDebug() << "pm size" << pm->size();

	QRect mainGeom = m_mainWindow->geometry();
	QRect mgeom = menu->geometry();
	QPoint mpos = menu->pos();
	QRect arect = menu->actionGeometry(qa);
	QSize menuRect = menu->size();
	QPoint p(mgeom.left()-mainGeom.left()+menuRect.width(),
			 mpos.y()-mainGeom.top()+arect.top());
	if (_INSDEBUG) qDebug() << "mainGeom" << mainGeom << "mgeom" << mgeom;
	if (_INSDEBUG) qDebug() << "mpos" << mpos << "arect" << arect << "p" << p;
	if (p.y() < 0)
		p.ry() = 0;
	if (_INSDEBUG) qDebug() << "p" << p << "mainGeom.height" << mainGeom.height() << "pm.height" << pm->height();
#define	SLOP	20
	if (p.y()+pm->height() > mainGeom.height()-SLOP)
		p.setY(mainGeom.height()-pm->height()-SLOP);
//	if (p.x()+pm->width() > mainGeom.width()) {
//		if (_INSDEBUG) qDebug() << "Move to left of menu" << (p.x()+pm->width()) << ">" << mainGeom.width();
//		p.rx() = mgeom.left()-mainGeom.left()-pm->width();
//	}
	if (_INSDEBUG) qDebug() << "final p" << p;
	m_preview->move(p);
}

QString InsertMenuMan::processSubstitutions(const QString& in)
{
	QRegExp re("\\@\\@(\\S+)\\@\\@");
	QStringList slist;
	int pos = 0;
	while ((pos = re.indexIn(in, pos)) != -1) {
		if (!slist.contains(re.cap(1)))
			slist << re.cap(1);
		pos += re.matchedLength();
	}
	qDebug() << "slist" << slist;
	if (slist.isEmpty())
		return(in);
	InsertSubstitutionsDialog sd(slist, m_mainWindow, this);
	int ret = sd.exec();
	if (ret != QDialog::Accepted)
		return(in);
	QString out = in;
	foreach (QString s, slist) {
		QRegExp rp("@@" + s + "@@");
		out = out.replace(rp, getSubstitution(s, &sd));
	}
	return(out);
}

QString	InsertMenuMan::getSubstitution(const QString& token, InsertSubstitutionsDialog* isd)
{
	if (!isd && token == "filename") {
		CodeEditor* ce = m_mainWindow->getCodeEditor();
		if (ce)
			return(ce->getFileName());
		return("***undefined***");
	} else if (!isd && token == "date") {
		QDateTime dt = QDateTime::currentDateTime();
		return(dt.toString("yyyyMMdd"));
	}
	if (isd) {
		int index = 0;
		foreach (QString s, isd->m_tokenList) {
			if (token == s)
				return(isd->m_lineEditList[index]->text());
			index++;
		}
	}
	return("");
}

///////////////////////////////////////////////////////////////////////////////
InsertSubstitutionsDialog::InsertSubstitutionsDialog(
		const QStringList& in, MainWindow* parent, InsertMenuMan* insertMenuMan)
	: QDialog(parent)
{
	m_tokenList = in;	// maybe sort the list
	setWindowTitle(tr("Substitute tokens"));
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(new QLabel(tr("Enter appropriate values for subsitution:")));
	QGridLayout* layout = new QGridLayout(this);
	int index = 0;
	foreach(QString s, in) {
		layout->addWidget(new QLabel(s), index, 0);
		QLineEdit* le = new QLineEdit(this);
		le->setText(insertMenuMan->getSubstitution(s, NULL));
		layout->addWidget(le, index, 1);
		m_lineEditList.append(le);
		index++;
	}
	mainLayout->addLayout(layout);
	QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
										 | QDialogButtonBox::Cancel);
	connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
	mainLayout->addWidget(buttonBox);
}

///////////////////////////////////////////////////////////////////////////////
InsertMenuPreview::InsertMenuPreview(QWidget* parent, Qt::WindowFlags f)
	: QWidget(parent, f)
{
	m_label = new QLabel(this);
	m_label->setMinimumSize(160,120);
	m_label->setParent(this);
}

