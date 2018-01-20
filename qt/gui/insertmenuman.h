/******************************************************************************
 * insertmenuman.h - Insert Menu Manager
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

#ifndef _INSERTMENUMAN_H_
#define _INSERTMENUMAN_H_

#include <QObject>
#include <QAction>
#include <QDialog>


QT_BEGIN_NAMESPACE
class QFileInfo;
class QMenu;
class QLabel;
class QLineEdit;
QT_END_NAMESPACE

class MainWindow;
class InsertMenuPreview;
class InsertSubstitutionsDialog;

class InsertMenuMan : public QObject
{
	Q_OBJECT

	friend class InsertSubstitutionsDialog;

public:
	explicit InsertMenuMan(MainWindow* parent);

	QString	normalizeIndenting(const QString& in);
	QString	determineLeadingIndent(const QString &leader);
	QString	applyIndent(const QString& in, const QString& indent);

	void populateMenu(bool enable);

signals:

public slots:
	void menuHidden();
	void itemHovered();
	void itemSelected();

private:
	QString	fixFileName(const QString& name);
	void	populateSubMenu(QMenu* menu, QFileInfo fi);
	QString	processSubstitutions(const QString& in);
	QString	getSubstitution(const QString& token, InsertSubstitutionsDialog* isd);


	MainWindow*	m_mainWindow;
	InsertMenuPreview*	m_preview;
};

class InsertMenuAction : public QAction
{
	Q_OBJECT

	friend class InsertMenuMan;

public:
	InsertMenuAction(QObject* parent)
		: QAction(parent) {}
	InsertMenuAction(const QString &text, QObject* parent)
		: QAction(text, parent) {}
	InsertMenuAction(const QIcon &icon, const QString &text, QObject *parent)
		: QAction(icon, text, parent) {}

private:
	QString	m_filePath;
};

class InsertMenuPreview : public QWidget
{
	Q_OBJECT
	friend class InsertMenuMan;

public:
	InsertMenuPreview(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());

private:
	QLabel*	m_label;
	QString	m_loadedFile;
};

class InsertSubstitutionsDialog : public QDialog
{
	friend class InsertMenuMan;
	Q_OBJECT
public:
	InsertSubstitutionsDialog(const QStringList& in, MainWindow* parent, InsertMenuMan *insertMenuMan);
	virtual ~InsertSubstitutionsDialog() {}

	QStringList			m_tokenList;
	QList<QLineEdit*>	m_lineEditList;
};
#endif // _INSERTMENUMAN_H_
